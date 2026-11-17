import React from 'react';
import { BookOpen, PlusCircle, History, RefreshCw } from 'lucide-react';

interface NavbarProps {
  onAddBookClick: () => void;
  onHistoryClick: () => void;
  onRefresh: () => void;
  isLoading: boolean;
}

export const Navbar: React.FC<NavbarProps> = ({
  onAddBookClick,
  onHistoryClick,
  onRefresh,
  isLoading,
}) => {
  return (
    <header className="bg-white border-b border-slate-200 sticky top-0 z-30 shadow-sm">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex justify-between items-center h-16">
          {/* Logo & Title */}
          <div className="flex items-center space-x-3">
            <div className="bg-blue-600 text-white p-2 rounded-lg shadow-sm">
              <BookOpen className="w-6 h-6" />
            </div>
            <div>
              <h1 className="text-xl font-bold text-slate-900 tracking-tight">
                Library Management System
              </h1>
              <p className="text-xs text-slate-500 font-medium">
                C++17 Drogon &bull; PostgreSQL &bull; React &bull; Tailwind
              </p>
            </div>
          </div>

          {/* Action Buttons */}
          <div className="flex items-center space-x-3">
            <button
              onClick={onRefresh}
              disabled={isLoading}
              title="Refresh Data"
              className="p-2 text-slate-600 hover:text-blue-600 hover:bg-slate-100 rounded-lg transition-colors border border-slate-200"
            >
              <RefreshCw className={`w-4 h-4 ${isLoading ? 'animate-spin text-blue-600' : ''}`} />
            </button>

            <button
              onClick={onHistoryClick}
              className="inline-flex items-center px-3.5 py-2 border border-slate-300 text-sm font-medium rounded-lg text-slate-700 bg-white hover:bg-slate-50 hover:border-slate-400 transition-colors shadow-sm"
            >
              <History className="w-4 h-4 mr-1.5 text-slate-500" />
              Transactions
            </button>

            <button
              onClick={onAddBookClick}
              className="inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-lg text-white bg-blue-600 hover:bg-blue-700 active:bg-blue-800 transition-colors shadow-sm"
            >
              <PlusCircle className="w-4 h-4 mr-1.5" />
              Add Book
            </button>
          </div>
        </div>
      </div>
    </header>
  );
};

