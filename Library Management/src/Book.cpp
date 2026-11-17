#include "Book.h"
#include "FileManager.h"
#include <iostream>
#include <iomanip>

Book::Book()
    : id(""), title(""), author(""), publisher(""), year(0), isAvailable(true) {}

Book::Book(std::string id, std::string title, std::string author,
           std::string publisher, int year, bool isAvailable)
    : id(std::move(id)),
      title(std::move(title)),
      author(std::move(author)),
      publisher(std::move(publisher)),
      year(year),
      isAvailable(isAvailable) {}

std::string Book::getId() const { return id; }
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
std::string Book::getPublisher() const { return publisher; }
int Book::getYear() const { return year; }
bool Book::getIsAvailable() const { return isAvailable; }

void Book::setTitle(const std::string& newTitle) { title = newTitle; }
void Book::setAuthor(const std::string& newAuthor) { author = newAuthor; }
void Book::setPublisher(const std::string& newPublisher) { publisher = newPublisher; }
void Book::setYear(int newYear) { year = newYear; }
void Book::setAvailable(bool status) { isAvailable = status; }

void Book::display() const {
    std::cout << "+---------------------+--------------------------------------------------------+\n";
    std::cout << "| Book ID             | " << std::left << std::setw(54) << id << " |\n";
    std::cout << "| Title               | " << std::left << std::setw(54) << title << " |\n";
    std::cout << "| Author              | " << std::left << std::setw(54) << author << " |\n";
    std::cout << "| Publisher           | " << std::left << std::setw(54) << publisher << " |\n";
    std::cout << "| Publication Year    | " << std::left << std::setw(54) << year << " |\n";
    std::cout << "| Status              | " << std::left << std::setw(54)
              << (isAvailable ? "Available" : "Issued") << " |\n";
    std::cout << "+---------------------+--------------------------------------------------------+\n";
}

std::string Book::toCSV() const {
    return FileManager::escapeCSVField(id) + "," +
           FileManager::escapeCSVField(title) + "," +
           FileManager::escapeCSVField(author) + "," +
           FileManager::escapeCSVField(publisher) + "," +
           std::to_string(year) + "," +
           (isAvailable ? "1" : "0");
}

Book Book::fromCSV(const std::string& line) {
    std::vector<std::string> fields = FileManager::parseCSVLine(line);
    if (fields.size() < 6) {
        return Book();
    }
    int yr = 0;
    try {
        yr = std::stoi(fields[4]);
    } catch (...) {
        yr = 0;
    }
    bool avail = (fields[5] == "1" || fields[5] == "true" || fields[5] == "Available");
    return Book(fields[0], fields[1], fields[2], fields[3], yr, avail);
}

