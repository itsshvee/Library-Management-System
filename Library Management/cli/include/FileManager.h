#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "Book.h"
#include "Transaction.h"
#include <vector>
#include <string>

class FileManager {
public:
    // Books persistence
    static bool loadBooks(const std::string& filepath, std::vector<Book>& books);
    static bool saveBooks(const std::string& filepath, const std::vector<Book>& books);

    // Transactions persistence
    static bool loadTransactions(const std::string& filepath, std::vector<Transaction>& transactions);
    static bool saveTransactions(const std::string& filepath, const std::vector<Transaction>& transactions);
    static bool appendTransaction(const std::string& filepath, const Transaction& transaction);

    // CSV parsing utilities
    static std::vector<std::string> parseCSVLine(const std::string& line);
    static std::string escapeCSVField(const std::string& field);
    
    // Directory management
    static void ensureDirectoryExists(const std::string& filepath);
};

#endif // FILE_MANAGER_H

