#include "Transaction.h"
#include "FileManager.h"

Transaction::Transaction()
    : transactionId(""), bookId(""), bookTitle(""), borrowerName(""), type(""), timestamp("") {}

Transaction::Transaction(std::string transId, std::string bId, std::string bTitle,
                         std::string borrower, std::string tType, std::string tStamp)
    : transactionId(std::move(transId)),
      bookId(std::move(bId)),
      bookTitle(std::move(bTitle)),
      borrowerName(std::move(borrower)),
      type(std::move(tType)),
      timestamp(std::move(tStamp)) {}

std::string Transaction::getTransactionId() const { return transactionId; }
std::string Transaction::getBookId() const { return bookId; }
std::string Transaction::getBookTitle() const { return bookTitle; }
std::string Transaction::getBorrowerName() const { return borrowerName; }
std::string Transaction::getType() const { return type; }
std::string Transaction::getTimestamp() const { return timestamp; }

std::string Transaction::toCSV() const {
    return FileManager::escapeCSVField(transactionId) + "," +
           FileManager::escapeCSVField(bookId) + "," +
           FileManager::escapeCSVField(bookTitle) + "," +
           FileManager::escapeCSVField(borrowerName) + "," +
           FileManager::escapeCSVField(type) + "," +
           FileManager::escapeCSVField(timestamp);
}

Transaction Transaction::fromCSV(const std::string& line) {
    std::vector<std::string> fields = FileManager::parseCSVLine(line);
    if (fields.size() < 6) {
        return Transaction();
    }
    return Transaction(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
}

