#include <drogon/drogon.h>
#include <iostream>

int main() {
    std::cout << "Starting Library Management System Backend (C++17 + Drogon)...\n";

    // Load configuration
    try {
        drogon::app().loadConfigFile("config.json");
    } catch (const std::exception& e) {
        std::cerr << "Warning: config.json failed to load (" << e.what() 
                  << "). Falling back to default settings.\n";
        drogon::app().addListener("0.0.0.0", 8080);
    }

    // Global CORS handler
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr&,
                                               const drogon::HttpResponsePtr& resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept, X-Requested-With");
    });

    // Health check endpoint
    drogon::app().registerHandler(
        "/api/health",
        [](const drogon::HttpRequestPtr&,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            Json::Value health;
            health["status"] = "UP";
            health["framework"] = "Drogon (C++17)";
            health["service"] = "Library Management System API";
            health["timestamp"] = drogon::app().getCustomConfig()["startTime"].asString();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(health);
            callback(resp);
        },
        {drogon::Get}
    );

    std::cout << "Drogon HTTP server listening on http://0.0.0.0:8080\n";
    drogon::app().run();
    return 0;
}

