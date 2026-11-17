#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "Transaction.h"
#include <vector>
#include <string>

class Library {
private:
    std::vector<Book> books;
    std::vector<Transaction> transactions;
    std::string booksFilePath;
    std::string transactionsFilePath;

    // Helper functions
    std::string generateTransactionId();
    std::string getCurrentDateTime() const;
    static std::string toLower(const std::string& str);
    static void printHeader();
    static void printDivider(char ch = '-', int len = 98);
    static void renderBookTable(const std::vector<Book>& bookList);

public:
    Library(std::string booksPath = "data/books.csv",
            std::string transactionsPath = "data/transactions.csv");

    // Lifecycle
    void loadData();
    void saveData() const;

    // Core operations
    void addBook(const Book& book);
    bool hasBook(const std::string& bookId) const;
    Book* findBookById(const std::string& bookId);
    const Book* findBookById(const std::string& bookId) const;

    // Display & views
    void displayAllBooks() const;
    void displayAvailableBooks() const;
    void displayIssuedBooks() const;

    // Search operations
    std::vector<Book> searchById(const std::string& id) const;
    std::vector<Book> searchByTitle(const std::string& title) const;
    std::vector<Book> searchByAuthor(const std::string& author) const;

    // Issue & Return
    void issueBook(const std::string& bookId, const std::string& borrowerName);
    void returnBook(const std::string& bookId);

    // Transaction history
    void displayTransactions() const;

    // Dashboard statistics
    size_t getTotalBooksCount() const;
    size_t getAvailableBooksCount() const;
    size_t getIssuedBooksCount() const;
    size_t getTotalTransactionsCount() const;
    void displayDashboard() const;
};

#endif // LIBRARY_H

