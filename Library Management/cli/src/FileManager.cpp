#include "FileManager.h"
#include "Exceptions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void FileManager::ensureDirectoryExists(const std::string& filepath) {
    try {
        fs::path p(filepath);
        if (p.has_parent_path()) {
            fs::create_directories(p.parent_path());
        }
    } catch (const std::exception& e) {
        throw FileOperationException("Failed to create directory structure for " + filepath + ": " + e.what());
    }
}

std::string FileManager::escapeCSVField(const std::string& field) {
    bool needsQuotes = false;
    if (field.find(',') != std::string::npos ||
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos ||
        field.find('\r') != std::string::npos) {
        needsQuotes = true;
    }

    if (!needsQuotes) {
        return field;
    }

    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    escaped += "\"";
    return escaped;
}

std::vector<std::string> FileManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '\r') {
            continue;
        }

        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i; // skip escaped quote
                } else {
                    inQuotes = false;
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                result.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
    }
    result.push_back(field);
    return result;
}

bool FileManager::loadBooks(const std::string& filepath, std::vector<Book>& books) {
    books.clear();
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ensureDirectoryExists(filepath);
        std::ofstream initFile(filepath);
        if (initFile.is_open()) {
            initFile << "BookID,Title,Author,Publisher,PublicationYear,IsAvailable\n";
            initFile.close();
        }
        return true;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (isFirstLine) {
            isFirstLine = false;
            if (line.rfind("BookID", 0) == 0 || line.rfind("ID", 0) == 0) {
                continue;
            }
        }
        try {
            Book b = Book::fromCSV(line);
            if (!b.getId().empty()) {
                books.push_back(b);
            }
        } catch (...) {
            continue;
        }
    }
    file.close();
    return true;
}

bool FileManager::saveBooks(const std::string& filepath, const std::vector<Book>& books) {
    ensureDirectoryExists(filepath);
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        throw FileOperationException("Unable to open file for writing: " + filepath);
    }

    file << "BookID,Title,Author,Publisher,PublicationYear,IsAvailable\n";
    for (const auto& book : books) {
        file << book.toCSV() << "\n";
    }
    file.close();
    return true;
}

bool FileManager::loadTransactions(const std::string& filepath, std::vector<Transaction>& transactions) {
    transactions.clear();
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ensureDirectoryExists(filepath);
        std::ofstream initFile(filepath);
        if (initFile.is_open()) {
            initFile << "TransactionID,BookID,BookTitle,BorrowerName,Type,Timestamp\n";
            initFile.close();
        }
        return true;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (isFirstLine) {
            isFirstLine = false;
            if (line.rfind("TransactionID", 0) == 0 || line.rfind("ID", 0) == 0) {
                continue;
            }
        }
        try {
            Transaction t = Transaction::fromCSV(line);
            if (!t.getTransactionId().empty()) {
                transactions.push_back(t);
            }
        } catch (...) {
            continue;
        }
    }
    file.close();
    return true;
}

bool FileManager::saveTransactions(const std::string& filepath, const std::vector<Transaction>& transactions) {
    ensureDirectoryExists(filepath);
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        throw FileOperationException("Unable to open file for writing: " + filepath);
    }

    file << "TransactionID,BookID,BookTitle,BorrowerName,Type,Timestamp\n";
    for (const auto& t : transactions) {
        file << t.toCSV() << "\n";
    }
    file.close();
    return true;
}

bool FileManager::appendTransaction(const std::string& filepath, const Transaction& transaction) {
    ensureDirectoryExists(filepath);
    bool needHeader = false;
    {
        std::ifstream checkFile(filepath);
        if (!checkFile.is_open() || checkFile.peek() == std::ifstream::traits_type::eof()) {
            needHeader = true;
        }
    }

    std::ofstream file(filepath, std::ios::app);
    if (!file.is_open()) {
        throw FileOperationException("Unable to append transaction to file: " + filepath);
    }

    if (needHeader) {
        file << "TransactionID,BookID,BookTitle,BorrowerName,Type,Timestamp\n";
    }
    file << transaction.toCSV() << "\n";
    file.close();
    return true;
}

