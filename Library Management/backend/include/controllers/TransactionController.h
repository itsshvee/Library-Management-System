#pragma once

#include <drogon/HttpController.h>

class TransactionController : public drogon::HttpController<TransactionController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TransactionController::issueBook, "/api/transactions/issue", drogon::Post, drogon::Options);
    ADD_METHOD_TO(TransactionController::returnBook, "/api/transactions/return", drogon::Post, drogon::Options);
    ADD_METHOD_TO(TransactionController::getTransactions, "/api/transactions", drogon::Get, drogon::Options);
    METHOD_LIST_END

    void issueBook(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void returnBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void getTransactions(const drogon::HttpRequestPtr& req,
                         std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

