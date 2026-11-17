#include "controllers/TransactionController.h"
#include "models/Transaction.h"
#include <drogon/drogon.h>
#include <chrono>
#include <iomanip>
#include <sstream>

static std::string generateTxnId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "TXN-" << ms;
    return oss.str();
}

void TransactionController::issueBook(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    auto json = req->getJsonObject();
    if (!json) {
        Json::Value err;
        err["error"] = "Invalid JSON payload";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    std::string bookId = (*json).get("bookId", "").asString();
    std::string borrower = (*json).get("borrowerName", "").asString();

    if (bookId.empty() || borrower.empty()) {
        Json::Value err;
        err["error"] = "Both 'bookId' and 'borrowerName' are required.";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    // Verify book existence and availability
    std::string checkSql = "SELECT id, title, is_available FROM books WHERE LOWER(id) = LOWER($1);";
    dbClient->execSqlAsync(
        checkSql,
        [callbackPtr, dbClient, bookId, borrower](const drogon::orm::Result& r) {
            if (r.empty()) {
                Json::Value err;
                err["error"] = "Book not found with ID: " + bookId;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k404NotFound);
                (*callbackPtr)(resp);
                return;
            }

            std::string canonicalId = r[0]["id"].as<std::string>();
            std::string bookTitle = r[0]["title"].as<std::string>();
            bool isAvail = r[0]["is_available"].as<bool>();

            if (!isAvail) {
                Json::Value err;
                err["error"] = "Book '" + canonicalId + "' is already issued and not currently available.";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k400BadRequest);
                (*callbackPtr)(resp);
                return;
            }

            std::string txnId = generateTxnId();

            // Perform issue transaction
            std::string updateSql = "UPDATE books SET is_available = FALSE, updated_at = NOW() WHERE id = $1;";
            dbClient->execSqlAsync(
                updateSql,
                [callbackPtr, dbClient, txnId, canonicalId, bookTitle, borrower](const drogon::orm::Result&) {
                    std::string insertTxnSql = 
                        "INSERT INTO transactions (transaction_id, book_id, book_title, borrower_name, type) "
                        "VALUES ($1, $2, $3, $4, 'ISSUE') "
                        "RETURNING transaction_id, book_id, book_title, borrower_name, type, "
                        "to_char(timestamp, 'YYYY-MM-DD HH24:MI:SS') as timestamp;";

                    dbClient->execSqlAsync(
                        insertTxnSql,
                        [callbackPtr](const drogon::orm::Result& txnRes) {
                            const auto& row = txnRes[0];
                            library::Transaction txn;
                            txn.transactionId = row["transaction_id"].as<std::string>();
                            txn.bookId = row["book_id"].as<std::string>();
                            txn.bookTitle = row["book_title"].as<std::string>();
                            txn.borrowerName = row["borrower_name"].as<std::string>();
                            txn.type = row["type"].as<std::string>();
                            txn.timestamp = row["timestamp"].as<std::string>();

                            auto resp = drogon::HttpResponse::newHttpJsonResponse(txn.toJson());
                            resp->setStatusCode(drogon::k201Created);
                            (*callbackPtr)(resp);
                        },
                        [callbackPtr](const drogon::orm::DrogonDbException& e) {
                            Json::Value err;
                            err["error"] = e.base().what();
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                            resp->setStatusCode(drogon::k500InternalServerError);
                            (*callbackPtr)(resp);
                        },
                        txnId, canonicalId, bookTitle, borrower
                    );
                },
                [callbackPtr](const drogon::orm::DrogonDbException& e) {
                    Json::Value err;
                    err["error"] = e.base().what();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                    resp->setStatusCode(drogon::k500InternalServerError);
                    (*callbackPtr)(resp);
                },
                canonicalId
            );
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        },
        bookId
    );
}

void TransactionController::returnBook(const drogon::HttpRequestPtr& req,
                                       std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    auto json = req->getJsonObject();
    if (!json) {
        Json::Value err;
        err["error"] = "Invalid JSON payload";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    std::string bookId = (*json).get("bookId", "").asString();
    if (bookId.empty()) {
        Json::Value err;
        err["error"] = "Field 'bookId' is required.";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    std::string checkSql = "SELECT id, title, is_available FROM books WHERE LOWER(id) = LOWER($1);";
    dbClient->execSqlAsync(
        checkSql,
        [callbackPtr, dbClient, bookId](const drogon::orm::Result& r) {
            if (r.empty()) {
                Json::Value err;
                err["error"] = "Book not found with ID: " + bookId;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k404NotFound);
                (*callbackPtr)(resp);
                return;
            }

            std::string canonicalId = r[0]["id"].as<std::string>();
            std::string bookTitle = r[0]["title"].as<std::string>();
            bool isAvail = r[0]["is_available"].as<bool>();

            if (isAvail) {
                Json::Value err;
                err["error"] = "Book '" + canonicalId + "' is already marked Available (not currently issued).";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k400BadRequest);
                (*callbackPtr)(resp);
                return;
            }

            // Find last borrower
            std::string findBorrowerSql = 
                "SELECT borrower_name FROM transactions WHERE book_id = $1 AND type = 'ISSUE' "
                "ORDER BY timestamp DESC LIMIT 1;";

            dbClient->execSqlAsync(
                findBorrowerSql,
                [callbackPtr, dbClient, canonicalId, bookTitle](const drogon::orm::Result& bRes) {
                    std::string borrower = bRes.empty() ? "Unknown Borrower" : bRes[0]["borrower_name"].as<std::string>();
                    std::string txnId = generateTxnId();

                    std::string updateSql = "UPDATE books SET is_available = TRUE, updated_at = NOW() WHERE id = $1;";
                    dbClient->execSqlAsync(
                        updateSql,
                        [callbackPtr, dbClient, txnId, canonicalId, bookTitle, borrower](const drogon::orm::Result&) {
                            std::string insertTxnSql = 
                                "INSERT INTO transactions (transaction_id, book_id, book_title, borrower_name, type) "
                                "VALUES ($1, $2, $3, $4, 'RETURN') "
                                "RETURNING transaction_id, book_id, book_title, borrower_name, type, "
                                "to_char(timestamp, 'YYYY-MM-DD HH24:MI:SS') as timestamp;";

                            dbClient->execSqlAsync(
                                insertTxnSql,
                                [callbackPtr](const drogon::orm::Result& txnRes) {
                                    const auto& row = txnRes[0];
                                    library::Transaction txn;
                                    txn.transactionId = row["transaction_id"].as<std::string>();
                                    txn.bookId = row["book_id"].as<std::string>();
                                    txn.bookTitle = row["book_title"].as<std::string>();
                                    txn.borrowerName = row["borrower_name"].as<std::string>();
                                    txn.type = row["type"].as<std::string>();
                                    txn.timestamp = row["timestamp"].as<std::string>();

                                    auto resp = drogon::HttpResponse::newHttpJsonResponse(txn.toJson());
                                    (*callbackPtr)(resp);
                                },
                                [callbackPtr](const drogon::orm::DrogonDbException& e) {
                                    Json::Value err;
                                    err["error"] = e.base().what();
                                    auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                                    resp->setStatusCode(drogon::k500InternalServerError);
                                    (*callbackPtr)(resp);
                                },
                                txnId, canonicalId, bookTitle, borrower
                            );
                        },
                        [callbackPtr](const drogon::orm::DrogonDbException& e) {
                            Json::Value err;
                            err["error"] = e.base().what();
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                            resp->setStatusCode(drogon::k500InternalServerError);
                            (*callbackPtr)(resp);
                        },
                        canonicalId
                    );
                },
                [callbackPtr](const drogon::orm::DrogonDbException& e) {
                    Json::Value err;
                    err["error"] = e.base().what();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                    resp->setStatusCode(drogon::k500InternalServerError);
                    (*callbackPtr)(resp);
                },
                canonicalId
            );
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        },
        bookId
    );
}

void TransactionController::getTransactions(const drogon::HttpRequestPtr& req,
                                           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    std::string sql = 
        "SELECT transaction_id, book_id, book_title, borrower_name, type, "
        "to_char(timestamp, 'YYYY-MM-DD HH24:MI:SS') as timestamp "
        "FROM transactions ORDER BY timestamp DESC;";

    dbClient->execSqlAsync(
        sql,
        [callbackPtr](const drogon::orm::Result& r) {
            Json::Value root(Json::arrayValue);
            for (const auto& row : r) {
                library::Transaction txn;
                txn.transactionId = row["transaction_id"].as<std::string>();
                txn.bookId = row["book_id"].as<std::string>();
                txn.bookTitle = row["book_title"].as<std::string>();
                txn.borrowerName = row["borrower_name"].as<std::string>();
                txn.type = row["type"].as<std::string>();
                txn.timestamp = row["timestamp"].as<std::string>();
                root.append(txn.toJson());
            }
            auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
            (*callbackPtr)(resp);
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        }
    );
}

