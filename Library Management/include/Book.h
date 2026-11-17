#ifndef BOOK_H
#define BOOK_H

#include <string>

class Book {
private:
    std::string id;
    std::string title;
    std::string author;
    std::string publisher;
    int year;
    bool isAvailable;

public:
    // Constructors
    Book();
    Book(std::string id, std::string title, std::string author,
         std::string publisher, int year, bool isAvailable = true);

    // Getters
    std::string getId() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getPublisher() const;
    int getYear() const;
    bool getIsAvailable() const;

    // Setters
    void setTitle(const std::string& title);
    void setAuthor(const std::string& author);
    void setPublisher(const std::string& publisher);
    void setYear(int year);
    void setAvailable(bool status);

    // Display
    void display() const;

    // CSV serialization
    std::string toCSV() const;
    static Book fromCSV(const std::string& line);
};

#endif // BOOK_H

