#pragma once

#include <string>
#include <json/json.h>

namespace library {

struct Transaction {
    std::string transactionId;
    std::string bookId;
    std::string bookTitle;
    std::string borrowerName;
    std::string type;
    std::string timestamp;

    Json::Value toJson() const {
        Json::Value json;
        json["transactionId"] = transactionId;
        json["bookId"] = bookId;
        json["bookTitle"] = bookTitle;
        json["borrowerName"] = borrowerName;
        json["type"] = type;
        json["timestamp"] = timestamp;
        return json;
    }
};

} // namespace library

