-- ==============================================================================
-- Library Management System - PostgreSQL Initialization Script
-- ==============================================================================

CREATE TABLE IF NOT EXISTS books (
    id VARCHAR(50) PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    author VARCHAR(255) NOT NULL,
    publisher VARCHAR(255) NOT NULL DEFAULT 'N/A',
    publication_year INTEGER NOT NULL CHECK (publication_year > 0 AND publication_year <= 2100),
    is_available BOOLEAN NOT NULL DEFAULT TRUE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS transactions (
    transaction_id VARCHAR(50) PRIMARY KEY,
    book_id VARCHAR(50) NOT NULL REFERENCES books(id) ON DELETE CASCADE,
    book_title VARCHAR(255) NOT NULL,
    borrower_name VARCHAR(255) NOT NULL,
    type VARCHAR(20) NOT NULL CHECK (type IN ('ISSUE', 'RETURN')),
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Indexes for performance
CREATE INDEX IF NOT EXISTS idx_books_title ON books(LOWER(title));
CREATE INDEX IF NOT EXISTS idx_books_author ON books(LOWER(author));
CREATE INDEX IF NOT EXISTS idx_books_available ON books(is_available);
CREATE INDEX IF NOT EXISTS idx_transactions_book_id ON transactions(book_id);
CREATE INDEX IF NOT EXISTS idx_transactions_timestamp ON transactions(timestamp DESC);

-- Seed Initial Books
INSERT INTO books (id, title, author, publisher, publication_year, is_available) VALUES
('B101', 'The C++ Programming Language', 'Bjarne Stroustrup', 'Addison-Wesley', 2013, TRUE),
('B102', 'Clean Code', 'Robert C. Martin', 'Prentice Hall', 2008, TRUE),
('B103', 'Design Patterns', 'Erich Gamma et al.', 'Addison-Wesley', 1994, TRUE),
('B104', 'Introduction to Algorithms', 'Thomas H. Cormen', 'MIT Press', 2009, FALSE),
('B105', 'The Pragmatic Programmer', 'Andrew Hunt & David Thomas', 'Addison-Wesley', 1999, TRUE),
('B106', 'Structure and Interpretation of Computer Programs', 'Harold Abelson', 'MIT Press', 1996, TRUE)
ON CONFLICT (id) DO NOTHING;

-- Seed Initial Transaction
INSERT INTO transactions (transaction_id, book_id, book_title, borrower_name, type, timestamp) VALUES
('TXN-00001', 'B104', 'Introduction to Algorithms', 'Alice Smith', 'ISSUE', NOW() - INTERVAL '1 day')
ON CONFLICT (transaction_id) DO NOTHING;

