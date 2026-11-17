import React from 'react';
import { Search, ArrowUpRight, ArrowDownLeft, Trash2, BookX } from 'lucide-react';
import { Book } from '../types';

interface BookTableProps {
  books: Book[];
  isLoading: boolean;
  searchTerm: string;
  onSearchChange: (value: string) => void;
  statusFilter: string;
  onStatusFilterChange: (status: 'all' | 'available' | 'issued') => void;
  onIssueClick: (book: Book) => void;
  onReturnClick: (book: Book) => void;
  onDeleteClick: (book: Book) => void;
}

export const BookTable: React.FC<BookTableProps> = ({
  books,
  isLoading,
  searchTerm,
  onSearchChange,
  statusFilter,
  onStatusFilterChange,
  onIssueClick,
  onReturnClick,
  onDeleteClick,
}) => {
  return (
    <div className="bg-white rounded-xl border border-slate-200 shadow-sm overflow-hidden">
      {/* Controls Bar */}
      <div className="p-4 sm:p-6 border-b border-slate-200 flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        {/* Search Input */}
        <div className="relative flex-1 max-w-md">
          <div className="absolute inset-y-0 left-0 pl-3.5 flex items-center pointer-events-none text-slate-400">
            <Search className="w-4 h-4" />
          </div>
          <input
            type="text"
            value={searchTerm}
            onChange={(e) => onSearchChange(e.target.value)}
            placeholder="Search by Title, Author, or Book ID..."
            className="block w-full pl-10 pr-4 py-2 border border-slate-300 rounded-lg text-sm placeholder-slate-400 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-transparent transition-all"
          />
        </div>

        {/* Filter Tabs */}
        <div className="flex bg-slate-100 p-1 rounded-lg self-start sm:self-auto">
          {(['all', 'available', 'issued'] as const).map((tab) => (
            <button
              key={tab}
              onClick={() => onStatusFilterChange(tab)}
              className={`px-3 py-1.5 text-xs font-semibold rounded-md transition-all capitalize ${
                statusFilter === tab
                  ? 'bg-white text-slate-900 shadow-sm'
                  : 'text-slate-600 hover:text-slate-900'
              }`}
            >
              {tab === 'all' ? 'All Books' : tab}
            </button>
          ))}
        </div>
      </div>

      {/* Table Container */}
      <div className="overflow-x-auto">
        <table className="min-w-full divide-y divide-slate-200 text-left text-sm">
          <thead className="bg-slate-50 text-slate-500 uppercase tracking-wider text-xs font-semibold">
            <tr>
              <th className="px-6 py-3.5">ID</th>
              <th className="px-6 py-3.5">Book Details</th>
              <th className="px-6 py-3.5">Author</th>
              <th className="px-6 py-3.5">Publisher & Year</th>
              <th className="px-6 py-3.5">Availability</th>
              <th className="px-6 py-3.5 text-right">Actions</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-slate-200 bg-white">
            {isLoading ? (
              <tr>
                <td colSpan={6} className="px-6 py-12 text-center text-slate-500">
                  <div className="inline-flex items-center space-x-2">
                    <div className="w-4 h-4 border-2 border-blue-600 border-t-transparent rounded-full animate-spin" />
                    <span>Loading library catalog...</span>
                  </div>
                </td>
              </tr>
            ) : books.length === 0 ? (
              <tr>
                <td colSpan={6} className="px-6 py-12 text-center">
                  <div className="flex flex-col items-center justify-center text-slate-400">
                    <BookX className="w-12 h-12 mb-3 stroke-1 text-slate-300" />
                    <p className="text-base font-semibold text-slate-700">No books found</p>
                    <p className="text-sm text-slate-500 mt-1">
                      {searchTerm
                        ? `No records matching "${searchTerm}". Try a different keyword.`
                        : 'No books match the selected filter.'}
                    </p>
                  </div>
                </td>
              </tr>
            ) : (
              books.map((book) => (
                <tr key={book.id} className="hover:bg-slate-50/80 transition-colors">
                  <td className="px-6 py-4 font-mono font-semibold text-slate-700 whitespace-nowrap">
                    {book.id}
                  </td>
                  <td className="px-6 py-4">
                    <div className="font-semibold text-slate-900">{book.title}</div>
                    <div className="text-xs text-slate-400">Added: {book.createdAt || 'N/A'}</div>
                  </td>
                  <td className="px-6 py-4 text-slate-700">{book.author}</td>
                  <td className="px-6 py-4 text-slate-600">
                    <span>{book.publisher}</span>
                    <span className="text-slate-400 text-xs ml-1.5">({book.year})</span>
                  </td>
                  <td className="px-6 py-4 whitespace-nowrap">
                    {book.isAvailable ? (
                      <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-semibold bg-emerald-50 text-emerald-700 border border-emerald-200">
                        <span className="w-1.5 h-1.5 mr-1.5 bg-emerald-500 rounded-full" />
                        Available
                      </span>
                    ) : (
                      <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-semibold bg-amber-50 text-amber-700 border border-amber-200">
                        <span className="w-1.5 h-1.5 mr-1.5 bg-amber-500 rounded-full" />
                        Issued
                      </span>
                    )}
                  </td>
                  <td className="px-6 py-4 whitespace-nowrap text-right">
                    <div className="flex items-center justify-end space-x-2">
                      {book.isAvailable ? (
                        <button
                          onClick={() => onIssueClick(book)}
                          className="inline-flex items-center px-3 py-1.5 text-xs font-medium rounded-lg text-emerald-700 bg-emerald-50 hover:bg-emerald-100 border border-emerald-200 transition-colors"
                        >
                          <ArrowUpRight className="w-3.5 h-3.5 mr-1" />
                          Issue
                        </button>
                      ) : (
                        <button
                          onClick={() => onReturnClick(book)}
                          className="inline-flex items-center px-3 py-1.5 text-xs font-medium rounded-lg text-blue-700 bg-blue-50 hover:bg-blue-100 border border-blue-200 transition-colors"
                        >
                          <ArrowDownLeft className="w-3.5 h-3.5 mr-1" />
                          Return
                        </button>
                      )}

                      <button
                        onClick={() => onDeleteClick(book)}
                        title="Delete Book"
                        className="p-1.5 text-slate-400 hover:text-red-600 hover:bg-red-50 rounded-lg transition-colors"
                      >
                        <Trash2 className="w-4 h-4" />
                      </button>
                    </div>
                  </td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      {/* Footer info */}
      <div className="px-6 py-3.5 bg-slate-50 border-t border-slate-200 text-xs text-slate-500 flex justify-between items-center">
        <span>Showing {books.length} book(s)</span>
        <span className="text-slate-400">Connected to PostgreSQL Database</span>
      </div>
    </div>
  );
};

