import { useEffect, useState, useCallback } from 'react';
import { Navbar } from './components/Navbar';
import { DashboardStats } from './components/DashboardStats';
import { BookTable } from './components/BookTable';
import { AddBookModal } from './components/AddBookModal';
import { IssueModal } from './components/IssueModal';
import { TransactionHistoryModal } from './components/TransactionHistoryModal';
import { Toast, ToastMessage } from './components/Toast';
import { api } from './services/api';
import { Book, CreateBookPayload, DashboardStats as StatsType, IssueBookPayload, Transaction } from './types';

export function App() {
  const [stats, setStats] = useState<StatsType | null>(null);
  const [books, setBooks] = useState<Book[]>([]);
  const [transactions, setTransactions] = useState<Transaction[]>([]);
  const [isLoadingBooks, setIsLoadingBooks] = useState(false);
  const [isLoadingStats, setIsLoadingStats] = useState(false);
  const [isLoadingTxns, setIsLoadingTxns] = useState(false);

  // Search & Filter state
  const [searchTerm, setSearchTerm] = useState('');
  const [statusFilter, setStatusFilter] = useState<'all' | 'available' | 'issued'>('all');

  // Modals state
  const [isAddModalOpen, setIsAddModalOpen] = useState(false);
  const [isIssueModalOpen, setIsIssueModalOpen] = useState(false);
  const [isHistoryModalOpen, setIsHistoryModalOpen] = useState(false);
  const [selectedBookForIssue, setSelectedBookForIssue] = useState<Book | null>(null);

  // Toasts
  const [toasts, setToasts] = useState<ToastMessage[]>([]);

  const addToast = (type: 'success' | 'error', message: string) => {
    const id = Date.now().toString() + Math.random().toString();
    setToasts((prev) => [...prev, { id, type, message }]);
  };

  const removeToast = (id: string) => {
    setToasts((prev) => prev.filter((t) => t.id !== id));
  };

  // Fetch Dashboard Stats
  const fetchStats = useCallback(async () => {
    try {
      setIsLoadingStats(true);
      const data = await api.getDashboardStats();
      setStats(data);
    } catch (err: unknown) {
      console.error('Failed to load stats:', err);
    } finally {
      setIsLoadingStats(false);
    }
  }, []);

  // Fetch Books
  const fetchBooks = useCallback(async () => {
    try {
      setIsLoadingBooks(true);
      const data = await api.getBooks(searchTerm, statusFilter);
      setBooks(data);
    } catch (err: unknown) {
      console.error('Failed to load books:', err);
      addToast('error', 'Unable to reach backend API. Ensure C++ Drogon service is running.');
    } finally {
      setIsLoadingBooks(false);
    }
  }, [searchTerm, statusFilter]);

  // Fetch Transactions
  const fetchTransactions = async () => {
    try {
      setIsLoadingTxns(true);
      const data = await api.getTransactions();
      setTransactions(data);
    } catch (err: unknown) {
      console.error('Failed to load transactions:', err);
      addToast('error', 'Failed to retrieve transaction history.');
    } finally {
      setIsLoadingTxns(false);
    }
  };

  // Initial load
  useEffect(() => {
    fetchStats();
    fetchBooks();
  }, [fetchStats, fetchBooks]);

  // Debounced search trigger
  useEffect(() => {
    const timer = setTimeout(() => {
      fetchBooks();
    }, 250);
    return () => clearTimeout(timer);
  }, [searchTerm, statusFilter, fetchBooks]);

  const handleRefreshAll = () => {
    fetchStats();
    fetchBooks();
  };

  // Handlers
  const handleCreateBook = async (payload: CreateBookPayload) => {
    await api.createBook(payload);
    addToast('success', `Book "${payload.title}" (ID: ${payload.id}) was successfully added.`);
    fetchBooks();
    fetchStats();
  };

  const handleOpenIssue = (book: Book) => {
    setSelectedBookForIssue(book);
    setIsIssueModalOpen(true);
  };

  const handleConfirmIssue = async (payload: IssueBookPayload) => {
    await api.issueBook(payload);
    addToast('success', `Book issued successfully to ${payload.borrowerName}.`);
    fetchBooks();
    fetchStats();
  };

  const handleReturnBook = async (book: Book) => {
    if (!confirm(`Return book "${book.title}" (ID: ${book.id}) to library catalog?`)) {
      return;
    }
    try {
      await api.returnBook({ bookId: book.id });
      addToast('success', `Book "${book.title}" was marked Available.`);
      fetchBooks();
      fetchStats();
    } catch (err: unknown) {
      if (err instanceof Error) {
        addToast('error', err.message);
      }
    }
  };

  const handleDeleteBook = async (book: Book) => {
    if (!confirm(`Are you sure you want to permanently delete "${book.title}" (ID: ${book.id})?`)) {
      return;
    }
    try {
      await api.deleteBook(book.id);
      addToast('success', `Book "${book.title}" was deleted.`);
      fetchBooks();
      fetchStats();
    } catch (err: unknown) {
      if (err instanceof Error) {
        addToast('error', err.message);
      }
    }
  };

  const handleOpenHistory = () => {
    fetchTransactions();
    setIsHistoryModalOpen(true);
  };

  return (
    <div className="min-h-screen flex flex-col bg-slate-50 text-slate-800">
      {/* Navigation */}
      <Navbar
        onAddBookClick={() => setIsAddModalOpen(true)}
        onHistoryClick={handleOpenHistory}
        onRefresh={handleRefreshAll}
        isLoading={isLoadingBooks || isLoadingStats}
      />

      {/* Main Content */}
      <main className="flex-1 max-w-7xl w-full mx-auto px-4 sm:px-6 lg:px-8 py-8">
        {/* Dashboard Cards */}
        <DashboardStats
          stats={stats}
          isLoading={isLoadingStats}
          activeFilter={statusFilter}
          onFilterChange={(f) => setStatusFilter(f)}
        />

        {/* Books Table */}
        <BookTable
          books={books}
          isLoading={isLoadingBooks}
          searchTerm={searchTerm}
          onSearchChange={setSearchTerm}
          statusFilter={statusFilter}
          onStatusFilterChange={setStatusFilter}
          onIssueClick={handleOpenIssue}
          onReturnClick={handleReturnBook}
          onDeleteClick={handleDeleteBook}
        />
      </main>

      {/* Footer */}
      <footer className="bg-white border-t border-slate-200 py-6">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 flex flex-col sm:flex-row items-center justify-between text-xs text-slate-500 gap-2">
          <p>
            &copy; {new Date().getFullYear()} Library Management System &bull; University C++ Engineering Practical
          </p>
          <div className="flex items-center space-x-4">
            <span className="inline-flex items-center text-emerald-600 font-medium">
              <span className="w-2 h-2 rounded-full bg-emerald-500 mr-1.5" />
              API: C++17 Drogon
            </span>
            <span className="inline-flex items-center text-blue-600 font-medium">
              <span className="w-2 h-2 rounded-full bg-blue-500 mr-1.5" />
              DB: PostgreSQL 16
            </span>
          </div>
        </div>
      </footer>

      {/* Modals */}
      <AddBookModal
        isOpen={isAddModalOpen}
        onClose={() => setIsAddModalOpen(false)}
        onSubmit={handleCreateBook}
      />

      <IssueModal
        isOpen={isIssueModalOpen}
        book={selectedBookForIssue}
        onClose={() => setIsIssueModalOpen(false)}
        onSubmit={handleConfirmIssue}
      />

      <TransactionHistoryModal
        isOpen={isHistoryModalOpen}
        transactions={transactions}
        isLoading={isLoadingTxns}
        onClose={() => setIsHistoryModalOpen(false)}
      />

      {/* Toasts */}
      <Toast toasts={toasts} onDismiss={removeToast} />
    </div>
  );
}


