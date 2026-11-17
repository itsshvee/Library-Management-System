#pragma once

#include <drogon/HttpController.h>

class DashboardController : public drogon::HttpController<DashboardController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(DashboardController::getStats, "/api/dashboard/stats", drogon::Get, drogon::Options);
    METHOD_LIST_END

    void getStats(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

