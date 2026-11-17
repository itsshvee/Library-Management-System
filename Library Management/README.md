# Full-Stack Library Management System
### Enterprise Distributed Architecture

A modern, high-performance, full-stack **Library Management System** built with **C++17 Drogon**, **React + TypeScript**, **Tailwind CSS**, and **PostgreSQL**, fully containerized via **Docker**.

---

## 1. System Technology Matrix

| Architectural Layer | Technology Stack | Purpose / Responsibilities |
| :--- | :--- | :--- |
| **Frontend UI** | **React 18 + TypeScript** | Modern single-page application (SPA), reactive state, type safety |
| **UI Styling** | **Tailwind CSS** | Responsive dashboard layout, clean borders, custom badges & modals |
| **Backend API** | **C++17 + Drogon Framework** | Ultra-high throughput non-blocking asynchronous HTTP REST server |
| **Database** | **PostgreSQL 16** | Relational data integrity, foreign keys, timestamps, indexes |
| **Database Management** | **pgAdmin 4** | Web-based GUI database administration and query analyzer |
| **API Documentation** | **Swagger / OpenAPI 3.0** | Interactive API specification and schema documentation |
| **API Testing** | **Postman Collection** | Pre-configured test suite for automated REST endpoint validation |
| **Containerization** | **Docker & Docker Compose** | Multi-container orchestration (`postgres`, `pgadmin`, `backend`, `frontend`) |
| **Version Control** | **Git & GitHub** | Distributed source tracking and repository structure |
| **Build System** | **CMake (C++) & Vite (Frontend)** | Modern declarative build and bundling pipeline |

---

## 2. Directory Structure

```text
Library Management/
├── backend/                                # C++17 Drogon Backend Service
│   ├── include/
│   │   ├── controllers/
│   │   │   ├── BookController.h            # CRUD endpoints for books
│   │   │   ├── TransactionController.h     # Issue/Return workflows
│   │   │   └── DashboardController.h       # Aggregated dashboard metrics
│   │   └── models/
│   │       ├── Book.h                      # Book DTO & JSON mapping
│   │       └── Transaction.h               # Transaction DTO
│   ├── src/
│   │   ├── BookController.cc
│   │   ├── TransactionController.cc
│   │   ├── DashboardController.cc
│   │   └── main.cc                         # Drogon listener, CORS & app lifecycle
│   ├── sql/
│   │   └── init.sql                        # PostgreSQL table creation & seed data
│   ├── CMakeLists.txt                      # C++ CMake configuration
│   ├── config.json                         # Drogon runtime & DB connection pool config
│   └── Dockerfile                          # Multi-stage C++ builder & runtime container
├── frontend/                               # React + TypeScript Frontend Service
│   ├── src/
│   │   ├── components/
│   │   │   ├── Navbar.tsx                  # Top header, actions, and status
│   │   │   ├── DashboardStats.tsx          # Real-time analytics metric cards
│   │   │   ├── BookTable.tsx               # Catalog table, filters & actions
│   │   │   ├── AddBookModal.tsx            # Form dialog to add books
│   │   │   ├── IssueModal.tsx              # Form dialog to borrow books
│   │   │   ├── TransactionHistoryModal.tsx # Full chronological audit modal
│   │   │   └── Toast.tsx                   # User feedback notifications
│   │   ├── services/
│   │   │   └── api.ts                      # Typed HTTP client for Drogon REST API
│   │   ├── types/
│   │   │   └── index.ts                    # TypeScript domain interfaces
│   │   ├── App.tsx                         # Core application controller
│   │   ├── main.tsx                        # Application mount
│   │   └── index.css                       # Tailwind CSS directives
│   ├── nginx.conf                          # Production Nginx reverse proxy
│   ├── package.json                        # Frontend dependencies
│   ├── vite.config.ts                      # Vite build & proxy config
│   ├── tailwind.config.js                  # Tailwind design tokens
│   ├── tsconfig.json                       # TypeScript compiler options
│   └── Dockerfile                          # Multi-stage Node builder & Nginx container
├── docs/                                   # Documentation & Tooling
│   ├── openapi.yaml                        # Swagger / OpenAPI 3.0 REST Specification
│   ├── Library_Management.postman_collection.json # Exported Postman test suite
│   ├── ARCHITECTURE.md                     # Deep architectural breakdown & ER diagram
│   └── UML.md                              # UML diagrams
├── cli/                                    # Standalone C++17 Console Application
├── docker-compose.yml                      # Full-stack multi-container orchestrator
└── README.md
```

---

## 3. Quickstart with Docker Compose

To build and run all 4 services (**Frontend, C++ Backend, PostgreSQL, pgAdmin**) simultaneously:

```bash
# 1. Clone repository and navigate to root directory
cd "/Users/kriss/Downloads/Library Management"

# 2. Build and launch containers in detached mode
docker-compose up --build
```

### Accessing the Running Services:

- **Web Frontend Dashboard:** [http://localhost:3000](http://localhost:3000)
- **C++ Drogon REST API:** [http://localhost:8080/api/books](http://localhost:8080/api/books)
- **API Health Endpoint:** [http://localhost:8080/api/health](http://localhost:8080/api/health)
- **pgAdmin 4 Console:** [http://localhost:5050](http://localhost:5050)
  - **Login Email:** `admin@library.com`
  - **Password:** `admin`
  - **Server Host:** `postgres`, Port: `5432`, Username: `postgres`, Password: `password`

---

## 4. Local Development Instructions

### A. Frontend (React + TypeScript)
```bash
cd frontend
npm install
npm run dev
# Vite runs at http://localhost:3000 with hot module reloading
```

### B. Backend (C++17 + Drogon)
Ensure PostgreSQL is running locally and `libpq` + `drogon` are installed:
```bash
cd backend
mkdir -p build && cd build
cmake ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
./library_backend
```

---

## 5. API Testing with Postman

1. Open **Postman**.
2. Click **Import** (top left).
3. Select the file: [docs/Library_Management.postman_collection.json](file:///Users/kriss/Downloads/Library%20Management/docs/Library_Management.postman_collection.json).
4. The collection includes ready-to-execute requests with pre-configured parameters:
   - `GET /api/health`
   - `GET /api/dashboard/stats`
   - `GET /api/books` (with query filters)
   - `POST /api/books`
   - `POST /api/transactions/issue`
   - `POST /api/transactions/return`
   - `GET /api/transactions`
   - `DELETE /api/books/:id`

---

## 6. API Documentation (Swagger / OpenAPI)

The full specification is defined in [docs/openapi.yaml](file:///Users/kriss/Downloads/Library%20Management/docs/openapi.yaml).
You can view it interactively by:
- Importing `docs/openapi.yaml` into [editor.swagger.io](https://editor.swagger.io) or the VS Code OpenAPI extension.

---

## 7. Database Management with pgAdmin

The `pgadmin` Docker service automatically boots alongside PostgreSQL.
To inspect tables and run queries:
1. Open `http://localhost:5050`.
2. Login with `admin@library.com` / `admin`.
3. Click **Add New Server**:
   - **Name:** `Library Database`
   - **Connection > Host:** `postgres`
   - **Maintenance database:** `library_db`
   - **Username:** `postgres`
   - **Password:** `password`
4. Inspect `books` and `transactions` tables with live constraints and indexes.

---

## 8. Version Control (Git & GitHub)

Repository initialization commands:
```bash
git init
git add .
git commit -m "feat: complete full-stack library management system (React, C++ Drogon, PostgreSQL, Docker)"
```
To connect to your GitHub remote repository:
```bash
git remote add origin https://github.com/<username>/<repo>.git
git branch -M main
git push -u origin main
```
