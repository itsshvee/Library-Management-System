# Enterprise Architecture Documentation

This document describes the multi-tier enterprise architecture of the **Library Management System** implemented with React, C++17 Drogon, PostgreSQL, and Docker.

---

## 1. System Architecture Diagram

```mermaid
flowchart TD
    subgraph ClientLayer["Presentation Layer (Port 3000)"]
        UI["React 18 + TypeScript + Tailwind CSS"]
        Nginx["Nginx Reverse Proxy / Web Server"]
        UI -->|Build artifacts| Nginx
    end

    subgraph APILayer["Application Layer (Port 8080)"]
        Drogon["C++17 Drogon REST HTTP Server"]
        BookCtrl["BookController"]
        TxnCtrl["TransactionController"]
        DashCtrl["DashboardController"]
        Cors["CORS & Advice Pipeline"]

        Drogon --> Cors
        Cors --> BookCtrl
        Cors --> TxnCtrl
        Cors --> DashCtrl
    end

    subgraph DataLayer["Persistence Layer (Port 5432 & 5050)"]
        Postgres[("PostgreSQL 16 Database\n(library_db)")]
        pgAdmin["pgAdmin 4 Web Console\n(Port 5050)"]
        pgAdmin -->|Administers| Postgres
    end

    subgraph TestingAndDocs["Tooling & Documentation"]
        Postman["Postman API Test Suite"]
        Swagger["OpenAPI 3.0 / Swagger Spec"]
    end

    Nginx -->|Reverse proxy /api/*| Drogon
    BookCtrl -->|Async libpq connection pool| Postgres
    TxnCtrl -->|Async libpq connection pool| Postgres
    DashCtrl -->|Async libpq connection pool| Postgres

    Postman -.->|Tests endpoints| Drogon
    Swagger -.->|Documents endpoints| Drogon
```

---

## 2. Database Entity-Relationship (ER) Diagram

```mermaid
erDiagram
    BOOKS ||--o{ TRANSACTIONS : "tracks history of"

    BOOKS {
        varchar(50) id PK "Unique Book Identifier (e.g. B101)"
        varchar(255) title "Full Title"
        varchar(255) author "Author Name"
        varchar(255) publisher "Publisher Name"
        integer publication_year "Year of Publication"
        boolean is_available "Current availability status"
        timestamptz created_at "Registration Timestamp"
        timestamptz updated_at "Last Modified Timestamp"
    }

    TRANSACTIONS {
        varchar(50) transaction_id PK "Unique Txn ID (e.g. TXN-1700000000)"
        varchar(50) book_id FK "References BOOKS(id) ON DELETE CASCADE"
        varchar(255) book_title "Snapshot of Book Title"
        varchar(255) borrower_name "Name of Member / Borrower"
        varchar(20) type "Transaction Type: ISSUE or RETURN"
        timestamptz timestamp "Date and time of transaction"
    }
```

---

## 3. Service Port Mapping

| Service | Technology | Port | Access URL | Credentials |
| :--- | :--- | :--- | :--- | :--- |
| **Frontend UI** | React + TypeScript + Tailwind | `3000` | `http://localhost:3000` | N/A |
| **Backend REST API** | C++17 + Drogon | `8080` | `http://localhost:8080/api` | N/A |
| **Database** | PostgreSQL 16 Alpine | `5432` | `localhost:5432` | User: `postgres`, Pass: `password`, DB: `library_db` |
| **DB Web Management**| pgAdmin 4 | `5050` | `http://localhost:5050` | Email: `admin@library.com`, Pass: `admin` |
| **API Specification**| Swagger / OpenAPI 3.0 | File | `docs/openapi.yaml` | N/A |
| **API Testing** | Postman Collection | File | `docs/Library_Management.postman_collection.json` | N/A |

---

## 4. REST API Endpoint Catalog

| HTTP Method | Route | Description | Request Body |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/health` | Healthcheck endpoint | None |
| `GET` | `/api/dashboard/stats` | Overall statistics for dashboard cards | None |
| `GET` | `/api/books` | List books (supports `?search=` and `?status=`) | None |
| `GET` | `/api/books/{id}` | Retrieve individual book details | None |
| `POST` | `/api/books` | Create new book record | `{ id, title, author, publisher, year }` |
| `PUT` | `/api/books/{id}` | Update book details | `{ title, author, publisher, year }` |
| `DELETE` | `/api/books/{id}` | Delete book record | None |
| `POST` | `/api/transactions/issue` | Issue book to borrower | `{ bookId, borrowerName }` |
| `POST` | `/api/transactions/return`| Return an issued book | `{ bookId }` |
| `GET` | `/api/transactions` | Full chronological audit trail | None |

