#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

enum class TransactionType {
    ISSUE,
    RETURN
};

class Transaction {
private:
    std::string transactionId;
    std::string bookId;
    std::string bookTitle;
    std::string borrowerName;
    std::string type;
    std::string timestamp;

public:
    Transaction();
    Transaction(std::string transId, std::string bId, std::string bTitle,
                std::string borrower, std::string tType, std::string tStamp);

    // Getters
    std::string getTransactionId() const;
    std::string getBookId() const;
    std::string getBookTitle() const;
    std::string getBorrowerName() const;
    std::string getType() const;
    std::string getTimestamp() const;

    // Serialization
    std::string toCSV() const;
    static Transaction fromCSV(const std::string& line);
};

#endif // TRANSACTION_H

