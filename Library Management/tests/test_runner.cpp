#include "Library.h"
#include "FileManager.h"
#include "Exceptions.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void runTests() {
    std::cout << "=====================================================\n";
    std::cout << "      RUNNING LIBRARY MANAGEMENT AUTOMATED TESTS     \n";
    std::cout << "=====================================================\n";

    std::string testBooksPath = "data/test_books.csv";
    std::string testTxnsPath = "data/test_transactions.csv";

    // Clean up test files if they exist
    if (fs::exists(testBooksPath)) fs::remove(testBooksPath);
    if (fs::exists(testTxnsPath)) fs::remove(testTxnsPath);

    // TEST 1: Add a new book
    std::cout << "[TEST 1] Add a single book...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        Book b1("B001", "The C++ Programming Language", "Bjarne Stroustrup", "Addison-Wesley", 2013, true);
        lib.addBook(b1);
        assert(lib.getTotalBooksCount() == 1);
        assert(lib.getAvailableBooksCount() == 1);
        assert(lib.hasBook("B001"));
        std::cout << " -> PASSED\n";
    }

    // TEST 2: Add multiple books
    std::cout << "[TEST 2] Add multiple books...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        Book b2("B002", "Effective Modern C++", "Scott Meyers", "O'Reilly", 2014, true);
        Book b3("B003", "Design Patterns", "Erich Gamma et al.", "Addison-Wesley", 1994, true);
        lib.addBook(b2);
        lib.addBook(b3);
        assert(lib.getTotalBooksCount() == 3);
        assert(lib.getAvailableBooksCount() == 3);
        std::cout << " -> PASSED\n";
    }

    // TEST 3: Try adding a duplicate book ID
    std::cout << "[TEST 3] Duplicate Book ID exception check...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        bool duplicateCaught = false;
        try {
            Book duplicate("B001", "Another Book", "Unknown", "Unknown", 2020, true);
            lib.addBook(duplicate);
        } catch (const DuplicateBookException& e) {
            duplicateCaught = true;
            std::cout << " Caught expected DuplicateBookException: " << e.what() << "\n";
        }
        assert(duplicateCaught);
        std::cout << " -> PASSED\n";
    }

    // TEST 4: Display all books
    std::cout << "[TEST 4] Display all books...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        lib.displayAllBooks();
        assert(lib.getTotalBooksCount() == 3);
        std::cout << " -> PASSED\n";
    }

    // TEST 5: Search by book ID
    std::cout << "[TEST 5] Search by Book ID...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        auto results = lib.searchById("B002");
        assert(results.size() == 1);
        assert(results[0].getTitle() == "Effective Modern C++");
        std::cout << " -> PASSED\n";
    }

    // TEST 6: Search by title
    std::cout << "[TEST 6] Search by Title keyword...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        auto results = lib.searchByTitle("Design Patterns");
        assert(results.size() == 1);
        assert(results[0].getId() == "B003");
        std::cout << " -> PASSED\n";
    }

    // TEST 7: Issue an available book
    std::cout << "[TEST 7] Issue an available book...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        lib.issueBook("B001", "Alice Johnson");
        assert(lib.getIssuedBooksCount() == 1);
        assert(lib.getAvailableBooksCount() == 2);
        assert(lib.getTotalTransactionsCount() == 1);
        Book* b = lib.findBookById("B001");
        assert(b != nullptr && !b->getIsAvailable());
        std::cout << " -> PASSED\n";
    }

    // TEST 8: Try issuing the same book again
    std::cout << "[TEST 8] Prevent issuing an already issued book...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        bool caught = false;
        try {
            lib.issueBook("B001", "Bob Smith");
        } catch (const BookAlreadyIssuedException& e) {
            caught = true;
            std::cout << " Caught expected BookAlreadyIssuedException: " << e.what() << "\n";
        }
        assert(caught);
        std::cout << " -> PASSED\n";
    }

    // TEST 9: Return an issued book
    std::cout << "[TEST 9] Return an issued book...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        lib.returnBook("B001");
        assert(lib.getIssuedBooksCount() == 0);
        assert(lib.getAvailableBooksCount() == 3);
        assert(lib.getTotalTransactionsCount() == 2);
        Book* b = lib.findBookById("B001");
        assert(b != nullptr && b->getIsAvailable());
        std::cout << " -> PASSED\n";
    }

    // TEST 10: Try returning an already returned book
    std::cout << "[TEST 10] Prevent returning an already available book...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        bool caught = false;
        try {
            lib.returnBook("B001");
        } catch (const BookAlreadyReturnedException& e) {
            caught = true;
            std::cout << " Caught expected BookAlreadyReturnedException: " << e.what() << "\n";
        }
        assert(caught);
        std::cout << " -> PASSED\n";
    }

    // TEST 11: Display transaction history
    std::cout << "[TEST 11] Display transaction history...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        lib.displayTransactions();
        assert(lib.getTotalTransactionsCount() == 2);
        std::cout << " -> PASSED\n";
    }

    // TEST 12: Save records
    std::cout << "[TEST 12] Explicitly save records...\n";
    {
        Library lib(testBooksPath, testTxnsPath);
        lib.saveData();
        assert(fs::exists(testBooksPath));
        assert(fs::exists(testTxnsPath));
        std::cout << " -> PASSED\n";
    }

    // TEST 13, 14, 15: Restart the application and verify persistence
    std::cout << "[TEST 13, 14, 15] Reload application and verify book & transaction persistence...\n";
    {
        Library reloadedLib(testBooksPath, testTxnsPath);
        assert(reloadedLib.getTotalBooksCount() == 3);
        assert(reloadedLib.getAvailableBooksCount() == 3);
        assert(reloadedLib.getIssuedBooksCount() == 0);
        assert(reloadedLib.getTotalTransactionsCount() == 2);

        Book* b = reloadedLib.findBookById("B001");
        assert(b != nullptr);
        assert(b->getTitle() == "The C++ Programming Language");
        assert(b->getAuthor() == "Bjarne Stroustrup");
        assert(b->getPublisher() == "Addison-Wesley");
        assert(b->getYear() == 2013);
        assert(b->getIsAvailable() == true);

        // Also test issue on reloaded data
        reloadedLib.issueBook("B002", "Charlie Brown");
        assert(reloadedLib.getIssuedBooksCount() == 1);
        assert(reloadedLib.getTotalTransactionsCount() == 3);
        std::cout << " -> PASSED\n";
    }

    // Clean up test files
    if (fs::exists(testBooksPath)) fs::remove(testBooksPath);
    if (fs::exists(testTxnsPath)) fs::remove(testTxnsPath);

    std::cout << "\n=====================================================\n";
    std::cout << "        ALL 15 UNIT & INTEGRATION TESTS PASSED!       \n";
    std::cout << "=====================================================\n";
}

int main() {
    try {
        runTests();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

