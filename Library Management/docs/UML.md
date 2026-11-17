# Library Management System - UML Documentation

This document provides architectural and behavioral UML diagrams for the **Library Management System** implemented in C++17.

---

## 1. Class Diagram

```mermaid
classDiagram
    direction TB

    class Book {
        -string id
        -string title
        -string author
        -string publisher
        -int year
        -bool isAvailable
        +Book()
        +Book(string id, string title, string author, string publisher, int year, bool isAvailable)
        +getId() string
        +getTitle() string
        +getAuthor() string
        +getPublisher() string
        +getYear() int
        +getIsAvailable() bool
        +setTitle(string title) void
        +setAuthor(string author) void
        +setPublisher(string publisher) void
        +setYear(int year) void
        +setAvailable(bool status) void
        +display() void
        +toCSV() string
        +fromCSV(string line)$ Book
    }

    class Transaction {
        -string transactionId
        -string bookId
        -string bookTitle
        -string borrowerName
        -string type
        -string timestamp
        +Transaction()
        +Transaction(string transId, string bId, string bTitle, string borrower, string tType, string tStamp)
        +getTransactionId() string
        +getBookId() string
        +getBookTitle() string
        +getBorrowerName() string
        +getType() string
        +getTimestamp() string
        +toCSV() string
        +fromCSV(string line)$ Transaction
    }

    class FileManager {
        <<utility>>
        +loadBooks(string filepath, vector~Book~& books)$ bool
        +saveBooks(string filepath, vector~Book~ books)$ bool
        +loadTransactions(string filepath, vector~Transaction~& transactions)$ bool
        +saveTransactions(string filepath, vector~Transaction~ transactions)$ bool
        +appendTransaction(string filepath, Transaction transaction)$ bool
        +parseCSVLine(string line)$ vector~string~
        +escapeCSVField(string field)$ string
        +ensureDirectoryExists(string filepath)$ void
    }

    class Library {
        -vector~Book~ books
        -vector~Transaction~ transactions
        -string booksFilePath
        -string transactionsFilePath
        -generateTransactionId() string
        -getCurrentDateTime() string
        -toLower(string str)$ string
        -renderBookTable(vector~Book~ bookList)$ void
        +Library(string booksPath, string transactionsPath)
        +loadData() void
        +saveData() void
        +addBook(Book book) void
        +hasBook(string bookId) bool
        +findBookById(string bookId) Book*
        +displayAllBooks() void
        +displayAvailableBooks() void
        +displayIssuedBooks() void
        +searchById(string id) vector~Book~
        +searchByTitle(string title) vector~Book~
        +searchByAuthor(string author) vector~Book~
        +issueBook(string bookId, string borrowerName) void
        +returnBook(string bookId) void
        +displayTransactions() void
        +getTotalBooksCount() size_t
        +getAvailableBooksCount() size_t
        +getIssuedBooksCount() size_t
        +getTotalTransactionsCount() size_t
        +displayDashboard() void
    }

    class LibraryException {
        +LibraryException(string message)
    }

    class DuplicateBookException {
        +DuplicateBookException(string id)
    }
    class BookNotFoundException {
        +BookNotFoundException(string id)
    }
    class BookAlreadyIssuedException {
        +BookAlreadyIssuedException(string id)
    }
    class BookAlreadyReturnedException {
        +BookAlreadyReturnedException(string id)
    }
    class InvalidInputException {
        +InvalidInputException(string message)
    }
    class FileOperationException {
        +FileOperationException(string message)
    }

    LibraryException <|-- DuplicateBookException
    LibraryException <|-- BookNotFoundException
    LibraryException <|-- BookAlreadyIssuedException
    LibraryException <|-- BookAlreadyReturnedException
    LibraryException <|-- InvalidInputException
    LibraryException <|-- FileOperationException

    Library "1" *-- "*" Book : manages
    Library "1" *-- "*" Transaction : logs
    Library ..> FileManager : uses
    Library ..> LibraryException : throws
```

---

## 2. Sequence Diagram: Book Issuance Flow

```mermaid
sequenceDiagram
    autonumber
    actor User as Librarian / User
    participant Main as UI / Main Loop
    participant Lib as Library System
    participant FM as FileManager
    participant Disk as CSV Storage (data/)

    User->>Main: Select "5. Issue Book"
    Main->>User: Prompt for Book ID & Borrower Name
    User->>Main: Enter "B101", "Alice Smith"
    Main->>Lib: issueBook("B101", "Alice Smith")
    
    alt Book ID not found
        Lib-->>Main: throw BookNotFoundException
        Main->>User: Display "[ERROR] Book with ID was not found."
    else Book is already issued
        Lib-->>Main: throw BookAlreadyIssuedException
        Main->>User: Display "[ERROR] Book is already issued."
    else Book is available
        Lib->>Lib: setAvailable(false)
        Lib->>Lib: generateTransactionId() ("TXN-0000X")
        Lib->>FM: saveBooks(booksFilePath, books)
        FM->>Disk: Overwrite books.csv
        Lib->>FM: appendTransaction(transactionsFilePath, txn)
        FM->>Disk: Append to transactions.csv
        Lib-->>Main: Success confirmation
        Main->>User: Display "[SUCCESS] Book has been successfully issued."
    end
```

---

## 3. Sequence Diagram: Book Return Flow

```mermaid
sequenceDiagram
    autonumber
    actor User as Librarian / User
    participant Main as UI / Main Loop
    participant Lib as Library System
    participant FM as FileManager
    participant Disk as CSV Storage (data/)

    User->>Main: Select "6. Return Book"
    Main->>User: Prompt for Book ID
    User->>Main: Enter "B101"
    Main->>Lib: returnBook("B101")
    
    alt Book ID not found
        Lib-->>Main: throw BookNotFoundException
        Main->>User: Display "[ERROR] Book with ID was not found."
    else Book is already returned / available
        Lib-->>Main: throw BookAlreadyReturnedException
        Main->>User: Display "[ERROR] Book is already available."
    else Book is currently issued
        Lib->>Lib: Resolve previous borrower from transaction log
        Lib->>Lib: setAvailable(true)
        Lib->>Lib: generateTransactionId()
        Lib->>FM: saveBooks(booksFilePath, books)
        FM->>Disk: Overwrite books.csv
        Lib->>FM: appendTransaction(transactionsFilePath, txn)
        FM->>Disk: Append to transactions.csv
        Lib-->>Main: Success confirmation
        Main->>User: Display "[SUCCESS] Book has been successfully returned."
    end
```

