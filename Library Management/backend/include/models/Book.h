#pragma once

#include <string>
#include <json/json.h>

namespace library {

struct Book {
    std::string id;
    std::string title;
    std::string author;
    std::string publisher;
    int year{0};
    bool isAvailable{true};
    std::string createdAt;
    std::string updatedAt;

    Json::Value toJson() const {
        Json::Value json;
        json["id"] = id;
        json["title"] = title;
        json["author"] = author;
        json["publisher"] = publisher;
        json["year"] = year;
        json["isAvailable"] = isAvailable;
        json["createdAt"] = createdAt;
        json["updatedAt"] = updatedAt;
        return json;
    }

    static Book fromJson(const Json::Value& json) {
        Book b;
        if (json.isMember("id") && json["id"].isString()) b.id = json["id"].asString();
        if (json.isMember("title") && json["title"].isString()) b.title = json["title"].asString();
        if (json.isMember("author") && json["author"].isString()) b.author = json["author"].asString();
        if (json.isMember("publisher") && json["publisher"].isString()) b.publisher = json["publisher"].asString();
        if (json.isMember("year") && json["year"].isInt()) b.year = json["year"].asInt();
        if (json.isMember("isAvailable") && json["isAvailable"].isBool()) b.isAvailable = json["isAvailable"].asBool();
        return b;
    }
};

} // namespace library

