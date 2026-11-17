#include "Library.h"
#include "Exceptions.h"
#include <iostream>
#include <string>
#include <limits>
#include <sstream>

namespace UI {
    void printTitle() {
        std::cout << "\n";
        std::cout << "===============================================================\n";
        std::cout << "                 LIBRARY MANAGEMENT SYSTEM                     \n";
        std::cout << "===============================================================\n";
    }

    void printMenu() {
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|                          MAIN MENU                          |\n";
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|  1. Dashboard Overview                                      |\n";
        std::cout << "|  2. Add New Book                                            |\n";
        std::cout << "|  3. View All Books                                          |\n";
        std::cout << "|  4. Search Book                                             |\n";
        std::cout << "|  5. Issue Book                                              |\n";
        std::cout << "|  6. Return Book                                             |\n";
        std::cout << "|  7. View Issued Books                                       |\n";
        std::cout << "|  8. View Available Books                                    |\n";
        std::cout << "|  9. Transaction History                                     |\n";
        std::cout << "| 10. Save Records                                            |\n";
        std::cout << "| 11. Exit                                                    |\n";
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "Enter your choice (1-11): ";
    }

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        if (!std::getline(std::cin, input)) {
            return "";
        }
        return trim(input);
    }

    int readInt(const std::string& prompt) {
        while (true) {
            std::string line = readLine(prompt);
            if (line.empty()) {
                throw InvalidInputException("Field cannot be empty.");
            }
            std::stringstream ss(line);
            int value;
            if (ss >> value && ss.eof()) {
                return value;
            }
            std::cout << "[!] Invalid number. Please enter a valid integer.\n";
        }
    }

    void printSuccess(const std::string& message) {
        std::cout << "\n[SUCCESS] " << message << "\n";
    }

    void printError(const std::string& message) {
        std::cout << "\n[ERROR] " << message << "\n";
    }

    void pause() {
        std::cout << "\nPress Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

void handleAddBook(Library& lib) {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "|                        ADD NEW BOOK                         |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::string id = UI::readLine("Enter Book ID: ");
    if (id.empty()) {
        UI::printError("Book ID cannot be blank.");
        return;
    }

    std::string title = UI::readLine("Enter Book Title: ");
    if (title.empty()) {
        UI::printError("Book Title cannot be blank.");
        return;
    }

    std::string author = UI::readLine("Enter Author Name: ");
    if (author.empty()) {
        UI::printError("Author Name cannot be blank.");
        return;
    }

    std::string publisher = UI::readLine("Enter Publisher: ");
    if (publisher.empty()) {
        publisher = "N/A";
    }

    int year = 0;
    try {
        year = UI::readInt("Enter Publication Year: ");
    } catch (const std::exception& e) {
        UI::printError(e.what());
        return;
    }

    try {
        Book newBook(id, title, author, publisher, year, true);
        lib.addBook(newBook);
        UI::printSuccess("Book '" + title + "' (ID: " + id + ") was successfully added!");
    } catch (const LibraryException& e) {
        UI::printError(e.what());
    } catch (const std::exception& e) {
        UI::printError(std::string("An unexpected error occurred: ") + e.what());
    }
}

void handleSearchBook(const Library& lib) {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "|                         SEARCH BOOK                         |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "1. Search by Book ID\n";
    std::cout << "2. Search by Title\n";
    std::cout << "3. Search by Author\n";
    std::cout << "Enter search mode (1-3): ";

    std::string choiceStr = UI::readLine("");
    if (choiceStr == "1") {
        std::string id = UI::readLine("Enter Book ID (or partial ID): ");
        auto results = lib.searchById(id);
        std::cout << "\n--- Search Results by ID: '" << id << "' ---\n";
        if (results.empty()) {
            std::cout << "No matching books found.\n";
        } else {
            for (const auto& b : results) {
                b.display();
            }
        }
    } else if (choiceStr == "2") {
        std::string title = UI::readLine("Enter Title (or keywords): ");
        auto results = lib.searchByTitle(title);
        std::cout << "\n--- Search Results by Title: '" << title << "' ---\n";
        if (results.empty()) {
            std::cout << "No matching books found.\n";
        } else {
            for (const auto& b : results) {
                b.display();
            }
        }
    } else if (choiceStr == "3") {
        std::string author = UI::readLine("Enter Author Name: ");
        auto results = lib.searchByAuthor(author);
        std::cout << "\n--- Search Results by Author: '" << author << "' ---\n";
        if (results.empty()) {
            std::cout << "No matching books found.\n";
        } else {
            for (const auto& b : results) {
                b.display();
            }
        }
    } else {
        UI::printError("Invalid search option selected.");
    }
}

void handleIssueBook(Library& lib) {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "|                         ISSUE BOOK                          |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::string id = UI::readLine("Enter Book ID to issue: ");
    if (id.empty()) {
        UI::printError("Book ID cannot be blank.");
        return;
    }

    std::string borrower = UI::readLine("Enter Borrower Name: ");
    if (borrower.empty()) {
        UI::printError("Borrower name cannot be blank.");
        return;
    }

    try {
        lib.issueBook(id, borrower);
        UI::printSuccess("Book (ID: " + id + ") has been successfully issued to " + borrower + ".");
    } catch (const LibraryException& e) {
        UI::printError(e.what());
    } catch (const std::exception& e) {
        UI::printError(std::string("An unexpected error occurred: ") + e.what());
    }
}

void handleReturnBook(Library& lib) {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "|                         RETURN BOOK                         |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::string id = UI::readLine("Enter Book ID to return: ");
    if (id.empty()) {
        UI::printError("Book ID cannot be blank.");
        return;
    }

    try {
        lib.returnBook(id);
        UI::printSuccess("Book (ID: " + id + ") has been successfully returned and is now Available.");
    } catch (const LibraryException& e) {
        UI::printError(e.what());
    } catch (const std::exception& e) {
        UI::printError(std::string("An unexpected error occurred: ") + e.what());
    }
}

int main() {
    try {
        Library library("data/books.csv", "data/transactions.csv");
        UI::printTitle();

        bool running = true;
        while (running) {
            UI::printMenu();
            std::string inputChoice = UI::readLine("");
            if (inputChoice.empty()) {
                continue;
            }

            int choice = -1;
            try {
                choice = std::stoi(inputChoice);
            } catch (...) {
                UI::printError("Please enter a valid menu number between 1 and 11.");
                UI::pause();
                continue;
            }

            switch (choice) {
                case 1:
                    library.displayDashboard();
                    UI::pause();
                    break;
                case 2:
                    handleAddBook(library);
                    UI::pause();
                    break;
                case 3:
                    library.displayAllBooks();
                    UI::pause();
                    break;
                case 4:
                    handleSearchBook(library);
                    UI::pause();
                    break;
                case 5:
                    handleIssueBook(library);
                    UI::pause();
                    break;
                case 6:
                    handleReturnBook(library);
                    UI::pause();
                    break;
                case 7:
                    library.displayIssuedBooks();
                    UI::pause();
                    break;
                case 8:
                    library.displayAvailableBooks();
                    UI::pause();
                    break;
                case 9:
                    library.displayTransactions();
                    UI::pause();
                    break;
                case 10:
                    try {
                        library.saveData();
                        UI::printSuccess("All records (books & transactions) saved to CSV files.");
                    } catch (const std::exception& e) {
                        UI::printError(e.what());
                    }
                    UI::pause();
                    break;
                case 11:
                    std::cout << "\nSaving records before exiting...\n";
                    try {
                        library.saveData();
                        UI::printSuccess("Records saved successfully.");
                    } catch (const std::exception& e) {
                        UI::printError(e.what());
                    }
                    std::cout << "Thank you for using Library Management System. Goodbye!\n\n";
                    running = false;
                    break;
                default:
                    UI::printError("Invalid selection! Please select a valid option from 1 to 11.");
                    UI::pause();
                    break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "\n[FATAL SYSTEM ERROR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}

