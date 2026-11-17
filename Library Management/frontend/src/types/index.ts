export interface Book {
  id: string;
  title: string;
  author: string;
  publisher: string;
  year: number;
  isAvailable: boolean;
  createdAt?: string;
  updatedAt?: string;
}

export interface Transaction {
  transactionId: string;
  bookId: string;
  bookTitle: string;
  borrowerName: string;
  type: 'ISSUE' | 'RETURN';
  timestamp: string;
}

export interface DashboardStats {
  totalBooks: number;
  availableBooks: number;
  issuedBooks: number;
  totalTransactions: number;
}

export interface CreateBookPayload {
  id: string;
  title: string;
  author: string;
  publisher: string;
  year: number;
}

export interface IssueBookPayload {
  bookId: string;
  borrowerName: string;
}

export interface ReturnBookPayload {
  bookId: string;
}

