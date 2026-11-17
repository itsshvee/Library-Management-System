#include "controllers/BookController.h"
#include "models/Book.h"
#include <drogon/drogon.h>

void BookController::getBooks(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    std::string search = req->getParameter("search");
    std::string status = req->getParameter("status"); // "all", "available", "issued"

    auto dbClient = drogon::app().getDbClient();
    std::string sql = "SELECT id, title, author, publisher, publication_year, is_available, "
                      "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
                      "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at "
                      "FROM books WHERE 1=1 ";

    std::vector<std::string> params;
    int paramIdx = 1;

    if (!search.empty()) {
        sql += "AND (LOWER(title) LIKE $" + std::to_string(paramIdx) + 
               " OR LOWER(author) LIKE $" + std::to_string(paramIdx) + 
               " OR LOWER(id) LIKE $" + std::to_string(paramIdx) + ") ";
        std::string searchPattern = "%";
        for (char c : search) searchPattern += static_cast<char>(std::tolower(c));
        searchPattern += "%";
        params.push_back(searchPattern);
        paramIdx++;
    }

    if (status == "available") {
        sql += "AND is_available = TRUE ";
    } else if (status == "issued") {
        sql += "AND is_available = FALSE ";
    }

    sql += "ORDER BY created_at DESC;";

    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    auto queryCallback = [callbackPtr](const drogon::orm::Result& r) {
        Json::Value root(Json::arrayValue);
        for (const auto& row : r) {
            library::Book b;
            b.id = row["id"].as<std::string>();
            b.title = row["title"].as<std::string>();
            b.author = row["author"].as<std::string>();
            b.publisher = row["publisher"].as<std::string>();
            b.year = row["publication_year"].as<int>();
            b.isAvailable = row["is_available"].as<bool>();
            b.createdAt = row["created_at"].as<std::string>();
            b.updatedAt = row["updated_at"].as<std::string>();
            root.append(b.toJson());
        }
        auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
        (*callbackPtr)(resp);
    };

    auto errorCallback = [callbackPtr](const drogon::orm::DrogonDbException& e) {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k500InternalServerError);
        (*callbackPtr)(resp);
    };

    if (params.empty()) {
        dbClient->execSqlAsync(sql, queryCallback, errorCallback);
    } else {
        dbClient->execSqlAsync(sql, queryCallback, errorCallback, params[0]);
    }
}

void BookController::getBookById(const drogon::HttpRequestPtr& req,
                                std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                const std::string& id) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    std::string sql = "SELECT id, title, author, publisher, publication_year, is_available, "
                      "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
                      "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at "
                      "FROM books WHERE LOWER(id) = LOWER($1);";

    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    dbClient->execSqlAsync(
        sql,
        [callbackPtr, id](const drogon::orm::Result& r) {
            if (r.empty()) {
                Json::Value err;
                err["error"] = "Book not found with ID: " + id;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k404NotFound);
                (*callbackPtr)(resp);
                return;
            }
            const auto& row = r[0];
            library::Book b;
            b.id = row["id"].as<std::string>();
            b.title = row["title"].as<std::string>();
            b.author = row["author"].as<std::string>();
            b.publisher = row["publisher"].as<std::string>();
            b.year = row["publication_year"].as<int>();
            b.isAvailable = row["is_available"].as<bool>();
            b.createdAt = row["created_at"].as<std::string>();
            b.updatedAt = row["updated_at"].as<std::string>();

            auto resp = drogon::HttpResponse::newHttpJsonResponse(b.toJson());
            (*callbackPtr)(resp);
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        },
        id
    );
}

void BookController::createBook(const drogon::HttpRequestPtr& req,
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
        err["error"] = "Invalid JSON request body";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    library::Book book = library::Book::fromJson(*json);
    if (book.id.empty() || book.title.empty() || book.author.empty()) {
        Json::Value err;
        err["error"] = "Fields 'id', 'title', and 'author' are required.";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    if (book.year <= 0 || book.year > 2100) {
        Json::Value err;
        err["error"] = "Publication year must be between 1 and 2100.";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    if (book.publisher.empty()) {
        book.publisher = "N/A";
    }

    auto dbClient = drogon::app().getDbClient();
    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    // Check duplicate ID
    std::string checkSql = "SELECT id FROM books WHERE LOWER(id) = LOWER($1);";
    dbClient->execSqlAsync(
        checkSql,
        [callbackPtr, dbClient, book](const drogon::orm::Result& r) {
            if (!r.empty()) {
                Json::Value err;
                err["error"] = "Book with ID '" + book.id + "' already exists.";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k409Conflict);
                (*callbackPtr)(resp);
                return;
            }

            // Insert
            std::string insertSql = 
                "INSERT INTO books (id, title, author, publisher, publication_year, is_available) "
                "VALUES ($1, $2, $3, $4, $5, $6) "
                "RETURNING id, title, author, publisher, publication_year, is_available, "
                "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
                "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at;";

            dbClient->execSqlAsync(
                insertSql,
                [callbackPtr](const drogon::orm::Result& insertResult) {
                    const auto& row = insertResult[0];
                    library::Book b;
                    b.id = row["id"].as<std::string>();
                    b.title = row["title"].as<std::string>();
                    b.author = row["author"].as<std::string>();
                    b.publisher = row["publisher"].as<std::string>();
                    b.year = row["publication_year"].as<int>();
                    b.isAvailable = row["is_available"].as<bool>();
                    b.createdAt = row["created_at"].as<std::string>();
                    b.updatedAt = row["updated_at"].as<std::string>();

                    auto resp = drogon::HttpResponse::newHttpJsonResponse(b.toJson());
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
                book.id, book.title, book.author, book.publisher, book.year, book.isAvailable
            );
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        },
        book.id
    );
}

void BookController::updateBook(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                               const std::string& id) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    auto json = req->getJsonObject();
    if (!json) {
        Json::Value err;
        err["error"] = "Invalid JSON request body";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    library::Book book = library::Book::fromJson(*json);
    auto dbClient = drogon::app().getDbClient();
    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    std::string sql = 
        "UPDATE books SET title = $1, author = $2, publisher = $3, publication_year = $4, updated_at = NOW() "
        "WHERE LOWER(id) = LOWER($5) "
        "RETURNING id, title, author, publisher, publication_year, is_available, "
        "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at;";

    dbClient->execSqlAsync(
        sql,
        [callbackPtr, id](const drogon::orm::Result& r) {
            if (r.empty()) {
                Json::Value err;
                err["error"] = "Book not found with ID: " + id;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k404NotFound);
                (*callbackPtr)(resp);
                return;
            }
            const auto& row = r[0];
            library::Book b;
            b.id = row["id"].as<std::string>();
            b.title = row["title"].as<std::string>();
            b.author = row["author"].as<std::string>();
            b.publisher = row["publisher"].as<std::string>();
            b.year = row["publication_year"].as<int>();
            b.isAvailable = row["is_available"].as<bool>();
            b.createdAt = row["created_at"].as<std::string>();
            b.updatedAt = row["updated_at"].as<std::string>();

            auto resp = drogon::HttpResponse::newHttpJsonResponse(b.toJson());
            (*callbackPtr)(resp);
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        },
        book.title, book.author, book.publisher, book.year, id
    );
}

void BookController::deleteBook(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                               const std::string& id) {
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k204NoContent);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    auto callbackPtr = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(std::move(callback));

    std::string sql = "DELETE FROM books WHERE LOWER(id) = LOWER($1) RETURNING id;";
    dbClient->execSqlAsync(
        sql,
        [callbackPtr, id](const drogon::orm::Result& r) {
            if (r.empty()) {
                Json::Value err;
                err["error"] = "Book not found with ID: " + id;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k404NotFound);
                (*callbackPtr)(resp);
                return;
            }
            Json::Value res;
            res["success"] = true;
            res["message"] = "Book '" + id + "' was successfully deleted.";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
            (*callbackPtr)(resp);
        },
        [callbackPtr](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = e.base().what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            (*callbackPtr)(resp);
        },
        id
    );
}

