#include "controllers/DashboardController.h"
#include <drogon/drogon.h>

void DashboardController::getStats(const drogon::HttpRequestPtr& req,
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
        "SELECT "
        "  (SELECT COUNT(*) FROM books) AS total_books, "
        "  (SELECT COUNT(*) FROM books WHERE is_available = TRUE) AS available_books, "
        "  (SELECT COUNT(*) FROM books WHERE is_available = FALSE) AS issued_books, "
        "  (SELECT COUNT(*) FROM transactions) AS total_transactions;";

    dbClient->execSqlAsync(
        sql,
        [callbackPtr](const drogon::orm::Result& r) {
            Json::Value stats;
            if (!r.empty()) {
                const auto& row = r[0];
                stats["totalBooks"] = static_cast<Json::Int64>(row["total_books"].as<long long>());
                stats["availableBooks"] = static_cast<Json::Int64>(row["available_books"].as<long long>());
                stats["issuedBooks"] = static_cast<Json::Int64>(row["issued_books"].as<long long>());
                stats["totalTransactions"] = static_cast<Json::Int64>(row["total_transactions"].as<long long>());
            } else {
                stats["totalBooks"] = 0;
                stats["availableBooks"] = 0;
                stats["issuedBooks"] = 0;
                stats["totalTransactions"] = 0;
            }
            auto resp = drogon::HttpResponse::newHttpJsonResponse(stats);
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

