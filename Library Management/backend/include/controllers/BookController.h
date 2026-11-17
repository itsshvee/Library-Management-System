#pragma once

#include <drogon/HttpController.h>

class BookController : public drogon::HttpController<BookController> {
public:
    METHOD_LIST_BEGIN
    // REST Book endpoints
    ADD_METHOD_TO(BookController::getBooks, "/api/books", drogon::Get, drogon::Options);
    ADD_METHOD_TO(BookController::getBookById, "/api/books/{id}", drogon::Get, drogon::Options);
    ADD_METHOD_TO(BookController::createBook, "/api/books", drogon::Post, drogon::Options);
    ADD_METHOD_TO(BookController::updateBook, "/api/books/{id}", drogon::Put, drogon::Options);
    ADD_METHOD_TO(BookController::deleteBook, "/api/books/{id}", drogon::Delete, drogon::Options);
    METHOD_LIST_END

    void getBooks(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void getBookById(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                     const std::string& id);

    void createBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void updateBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    const std::string& id);

    void deleteBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    const std::string& id);
};

