import { Book, CreateBookPayload, DashboardStats, IssueBookPayload, ReturnBookPayload, Transaction } from '../types';

const API_BASE = '/api';

export class ApiError extends Error {
  status: number;
  constructor(message: string, status: number) {
    super(message);
    this.status = status;
    this.name = 'ApiError';
  }
}

async function request<T>(url: string, options: RequestInit = {}): Promise<T> {
  const headers = {
    'Content-Type': 'application/json',
    ...(options.headers || {}),
  };

  const response = await fetch(`${API_BASE}${url}`, {
    ...options,
    headers,
  });

  if (!response.ok) {
    let errorMsg = `HTTP Error ${response.status}`;
    try {
      const errorJson = await response.json();
      errorMsg = errorJson.error || errorJson.message || errorMsg;
    } catch {
      // Non-JSON error body
    }
    throw new ApiError(errorMsg, response.status);
  }

  if (response.status === 204) {
    return {} as T;
  }

  return response.json();
}

export const api = {
  // Dashboard
  getDashboardStats: () => request<DashboardStats>('/dashboard/stats'),

  // Books
  getBooks: (search = '', status = 'all') => {
    const params = new URLSearchParams();
    if (search) params.append('search', search);
    if (status && status !== 'all') params.append('status', status);
    const queryString = params.toString() ? `?${params.toString()}` : '';
    return request<Book[]>(`/books${queryString}`);
  },

  getBookById: (id: string) => request<Book>(`/books/${encodeURIComponent(id)}`),

  createBook: (payload: CreateBookPayload) =>
    request<Book>('/books', {
      method: 'POST',
      body: JSON.stringify(payload),
    }),

  updateBook: (id: string, payload: Partial<Book>) =>
    request<Book>(`/books/${encodeURIComponent(id)}`, {
      method: 'PUT',
      body: JSON.stringify(payload),
    }),

  deleteBook: (id: string) =>
    request<{ success: boolean; message: string }>(`/books/${encodeURIComponent(id)}`, {
      method: 'DELETE',
    }),

  // Transactions
  issueBook: (payload: IssueBookPayload) =>
    request<Transaction>('/transactions/issue', {
      method: 'POST',
      body: JSON.stringify(payload),
    }),

  returnBook: (payload: ReturnBookPayload) =>
    request<Transaction>('/transactions/return', {
      method: 'POST',
      body: JSON.stringify(payload),
    }),

  getTransactions: () => request<Transaction[]>('/transactions'),
};

