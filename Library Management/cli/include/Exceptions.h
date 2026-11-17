#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class LibraryException : public std::runtime_error {
public:
    explicit LibraryException(const std::string& message)
        : std::runtime_error(message) {}
};

class DuplicateBookException : public LibraryException {
public:
    explicit DuplicateBookException(const std::string& id)
        : LibraryException("Book with ID '" + id + "' already exists in the catalog.") {}
};

class BookNotFoundException : public LibraryException {
public:
    explicit BookNotFoundException(const std::string& id)
        : LibraryException("Book with ID '" + id + "' was not found.") {}
};

class BookAlreadyIssuedException : public LibraryException {
public:
    explicit BookAlreadyIssuedException(const std::string& id)
        : LibraryException("Book with ID '" + id + "' is already issued and not available.") {}
};

class BookAlreadyReturnedException : public LibraryException {
public:
    explicit BookAlreadyReturnedException(const std::string& id)
        : LibraryException("Book with ID '" + id + "' is already available (not currently issued).") {}
};

class InvalidInputException : public LibraryException {
public:
    explicit InvalidInputException(const std::string& message)
        : LibraryException("Invalid input: " + message) {}
};

class FileOperationException : public LibraryException {
public:
    explicit FileOperationException(const std::string& message)
        : LibraryException("File error: " + message) {}
};

#endif // EXCEPTIONS_H

