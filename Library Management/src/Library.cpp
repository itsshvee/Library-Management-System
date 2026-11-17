#include "Library.h"
#include "FileManager.h"
#include "Exceptions.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>

Library::Library(std::string booksPath, std::string transactionsPath)
    : booksFilePath(std::move(booksPath)),
      transactionsFilePath(std::move(transactionsPath)) {
    loadData();
}

std::string Library::toLower(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return lower;
}

std::string Library::getCurrentDateTime() const {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm tmStruct{};
#if defined(_WIN32)
    localtime_s(&tmStruct, &nowTime);
#else
    localtime_r(&nowTime, &tmStruct);
#endif
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tmStruct);
    return std::string(buffer);
}

std::string Library::generateTransactionId() {
    int nextSeq = static_cast<int>(transactions.size()) + 1;
    std::ostringstream oss;
    oss << "TXN-" << std::setw(5) << std::setfill('0') << nextSeq;
    return oss.str();
}

void Library::loadData() {
    FileManager::loadBooks(booksFilePath, books);
    FileManager::loadTransactions(transactionsFilePath, transactions);
}

void Library::saveData() const {
    FileManager::saveBooks(booksFilePath, books);
    FileManager::saveTransactions(transactionsFilePath, transactions);
}

bool Library::hasBook(const std::string& bookId) const {
    std::string lowerId = toLower(bookId);
    return std::any_of(books.begin(), books.end(), [&](const Book& b) {
        return toLower(b.getId()) == lowerId;
    });
}

Book* Library::findBookById(const std::string& bookId) {
    std::string lowerId = toLower(bookId);
    auto it = std::find_if(books.begin(), books.end(), [&](const Book& b) {
        return toLower(b.getId()) == lowerId;
    });
    if (it != books.end()) {
        return &(*it);
    }
    return nullptr;
}

const Book* Library::findBookById(const std::string& bookId) const {
    std::string lowerId = toLower(bookId);
    auto it = std::find_if(books.begin(), books.end(), [&](const Book& b) {
        return toLower(b.getId()) == lowerId;
    });
    if (it != books.end()) {
        return &(*it);
    }
    return nullptr;
}

void Library::addBook(const Book& book) {
    if (book.getId().empty()) {
        throw InvalidInputException("Book ID cannot be empty.");
    }
    if (book.getTitle().empty()) {
        throw InvalidInputException("Book Title cannot be empty.");
    }
    if (book.getAuthor().empty()) {
        throw InvalidInputException("Author Name cannot be empty.");
    }
    if (book.getYear() <= 0 || book.getYear() > 2100) {
        throw InvalidInputException("Publication Year must be a realistic valid year (1-2100).");
    }

    if (hasBook(book.getId())) {
        throw DuplicateBookException(book.getId());
    }

    books.push_back(book);
    FileManager::saveBooks(booksFilePath, books);
}

void Library::renderBookTable(const std::vector<Book>& bookList) {
    if (bookList.empty()) {
        std::cout << "\n+--------------------------------------------------------------------------------------------------+\n";
        std::cout << "| No books found matching the criteria.                                                            |\n";
        std::cout << "+--------------------------------------------------------------------------------------------------+\n";
        return;
    }

    std::cout << "+----------+--------------------------------+----------------------+--------------------+------+------------+\n";
    std::cout << "| " << std::left << std::setw(8) << "Book ID"
              << " | " << std::setw(30) << "Title"
              << " | " << std::setw(20) << "Author"
              << " | " << std::setw(18) << "Publisher"
              << " | " << std::setw(4) << "Year"
              << " | " << std::setw(10) << "Status" << " |\n";
    std::cout << "+----------+--------------------------------+----------------------+--------------------+------+------------+\n";

    for (const auto& b : bookList) {
        std::string title = b.getTitle();
        if (title.length() > 30) {
            title = title.substr(0, 27) + "...";
        }
        std::string author = b.getAuthor();
        if (author.length() > 20) {
            author = author.substr(0, 17) + "...";
        }
        std::string pub = b.getPublisher();
        if (pub.length() > 18) {
            pub = pub.substr(0, 15) + "...";
        }

        std::string status = b.getIsAvailable() ? "Available" : "Issued";

        std::cout << "| " << std::left << std::setw(8) << b.getId()
                  << " | " << std::setw(30) << title
                  << " | " << std::setw(20) << author
                  << " | " << std::setw(18) << pub
                  << " | " << std::setw(4) << b.getYear()
                  << " | " << std::setw(10) << status << " |\n";
    }
    std::cout << "+----------+--------------------------------+----------------------+--------------------+------+------------+\n";
    std::cout << " Total Count: " << bookList.size() << " book(s)\n";
}

void Library::displayAllBooks() const {
    std::cout << "\n======================================== ALL REGISTERED BOOKS ======================================\n";
    renderBookTable(books);
}

void Library::displayAvailableBooks() const {
    std::vector<Book> available;
    std::copy_if(books.begin(), books.end(), std::back_inserter(available),
                 [](const Book& b) { return b.getIsAvailable(); });

    std::cout << "\n======================================= CURRENTLY AVAILABLE BOOKS ==================================\n";
    renderBookTable(available);
}

void Library::displayIssuedBooks() const {
    std::vector<Book> issued;
    std::copy_if(books.begin(), books.end(), std::back_inserter(issued),
                 [](const Book& b) { return !b.getIsAvailable(); });

    std::cout << "\n========================================= CURRENTLY ISSUED BOOKS ===================================\n";
    renderBookTable(issued);
}

std::vector<Book> Library::searchById(const std::string& id) const {
    std::string query = toLower(id);
    std::vector<Book> results;
    for (const auto& b : books) {
        if (toLower(b.getId()).find(query) != std::string::npos) {
            results.push_back(b);
        }
    }
    return results;
}

std::vector<Book> Library::searchByTitle(const std::string& title) const {
    std::string query = toLower(title);
    std::vector<Book> results;
    for (const auto& b : books) {
        if (toLower(b.getTitle()).find(query) != std::string::npos) {
            results.push_back(b);
        }
    }
    return results;
}

std::vector<Book> Library::searchByAuthor(const std::string& author) const {
    std::string query = toLower(author);
    std::vector<Book> results;
    for (const auto& b : books) {
        if (toLower(b.getAuthor()).find(query) != std::string::npos) {
            results.push_back(b);
        }
    }
    return results;
}

void Library::issueBook(const std::string& bookId, const std::string& borrowerName) {
    if (bookId.empty()) {
        throw InvalidInputException("Book ID cannot be empty.");
    }
    if (borrowerName.empty()) {
        throw InvalidInputException("Borrower name cannot be empty.");
    }

    Book* book = findBookById(bookId);
    if (!book) {
        throw BookNotFoundException(bookId);
    }

    if (!book->getIsAvailable()) {
        throw BookAlreadyIssuedException(bookId);
    }

    book->setAvailable(false);

    std::string transId = generateTransactionId();
    std::string timestamp = getCurrentDateTime();
    Transaction txn(transId, book->getId(), book->getTitle(), borrowerName, "ISSUE", timestamp);
    transactions.push_back(txn);

    FileManager::saveBooks(booksFilePath, books);
    FileManager::appendTransaction(transactionsFilePath, txn);
}

void Library::returnBook(const std::string& bookId) {
    if (bookId.empty()) {
        throw InvalidInputException("Book ID cannot be empty.");
    }

    Book* book = findBookById(bookId);
    if (!book) {
        throw BookNotFoundException(bookId);
    }

    if (book->getIsAvailable()) {
        throw BookAlreadyReturnedException(bookId);
    }

    std::string borrower = "Unknown";
    for (auto it = transactions.rbegin(); it != transactions.rend(); ++it) {
        if (toLower(it->getBookId()) == toLower(bookId) && it->getType() == "ISSUE") {
            borrower = it->getBorrowerName();
            break;
        }
    }

    book->setAvailable(true);

    std::string transId = generateTransactionId();
    std::string timestamp = getCurrentDateTime();
    Transaction txn(transId, book->getId(), book->getTitle(), borrower, "RETURN", timestamp);
    transactions.push_back(txn);

    FileManager::saveBooks(booksFilePath, books);
    FileManager::appendTransaction(transactionsFilePath, txn);
}

void Library::displayTransactions() const {
    std::cout << "\n====================================== TRANSACTION AUDIT TRAIL =====================================\n";
    if (transactions.empty()) {
        std::cout << "+--------------------------------------------------------------------------------------------------+\n";
        std::cout << "| No transaction records found.                                                                    |\n";
        std::cout << "+--------------------------------------------------------------------------------------------------+\n";
        return;
    }

    std::cout << "+-----------+----------+--------------------------------+--------------------+--------+---------------------+\n";
    std::cout << "| " << std::left << std::setw(9) << "Txn ID"
              << " | " << std::setw(8) << "Book ID"
              << " | " << std::setw(30) << "Book Title"
              << " | " << std::setw(18) << "Borrower"
              << " | " << std::setw(6) << "Type"
              << " | " << std::setw(19) << "Date & Time" << " |\n";
    std::cout << "+-----------+----------+--------------------------------+--------------------+--------+---------------------+\n";

    for (const auto& t : transactions) {
        std::string title = t.getBookTitle();
        if (title.length() > 30) {
            title = title.substr(0, 27) + "...";
        }
        std::string borrower = t.getBorrowerName();
        if (borrower.length() > 18) {
            borrower = borrower.substr(0, 15) + "...";
        }

        std::cout << "| " << std::left << std::setw(9) << t.getTransactionId()
                  << " | " << std::setw(8) << t.getBookId()
                  << " | " << std::setw(30) << title
                  << " | " << std::setw(18) << borrower
                  << " | " << std::setw(6) << t.getType()
                  << " | " << std::setw(19) << t.getTimestamp() << " |\n";
    }
    std::cout << "+-----------+----------+--------------------------------+--------------------+--------+---------------------+\n";
    std::cout << " Total Transactions: " << transactions.size() << "\n";
}

size_t Library::getTotalBooksCount() const {
    return books.size();
}

size_t Library::getAvailableBooksCount() const {
    return std::count_if(books.begin(), books.end(), [](const Book& b) {
        return b.getIsAvailable();
    });
}

size_t Library::getIssuedBooksCount() const {
    return std::count_if(books.begin(), books.end(), [](const Book& b) {
        return !b.getIsAvailable();
    });
}

size_t Library::getTotalTransactionsCount() const {
    return transactions.size();
}

void Library::displayDashboard() const {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "|                     DASHBOARD OVERVIEW                      |\n";
    std::cout << "+-----------------------------------+-------------------------+\n";
    std::cout << "| Metric                            | Count                   |\n";
    std::cout << "+-----------------------------------+-------------------------+\n";
    std::cout << "| Total Registered Books            | " << std::left << std::setw(23) << getTotalBooksCount() << " |\n";
    std::cout << "| Available Books                   | " << std::left << std::setw(23) << getAvailableBooksCount() << " |\n";
    std::cout << "| Currently Issued Books            | " << std::left << std::setw(23) << getIssuedBooksCount() << " |\n";
    std::cout << "| Total Transactions Recorded       | " << std::left << std::setw(23) << getTotalTransactionsCount() << " |\n";
    std::cout << "+-----------------------------------+-------------------------+\n";
}

