import React, { useState } from 'react';
import { X, ArrowUpRight } from 'lucide-react';
import { Book, IssueBookPayload } from '../types';

interface IssueModalProps {
  isOpen: boolean;
  book: Book | null;
  onClose: () => void;
  onSubmit: (payload: IssueBookPayload) => Promise<void>;
}

export const IssueModal: React.FC<IssueModalProps> = ({
  isOpen,
  book,
  onClose,
  onSubmit,
}) => {
  const [borrowerName, setBorrowerName] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [error, setError] = useState<string | null>(null);

  if (!isOpen || !book) return null;

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!borrowerName.trim()) {
      setError('Borrower name cannot be blank.');
      return;
    }

    try {
      setIsSubmitting(true);
      setError(null);
      await onSubmit({
        bookId: book.id,
        borrowerName: borrowerName.trim(),
      });
      setBorrowerName('');
      onClose();
    } catch (err: unknown) {
      if (err instanceof Error) {
        setError(err.message);
      } else {
        setError('Failed to issue book.');
      }
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="fixed inset-0 z-50 overflow-y-auto bg-slate-900/50 backdrop-blur-sm flex items-center justify-center p-4">
      <div className="bg-white rounded-2xl max-w-md w-full shadow-2xl border border-slate-100 overflow-hidden animate-in fade-in zoom-in-95 duration-150">
        <div className="px-6 py-4 border-b border-slate-200 flex items-center justify-between bg-slate-50">
          <div className="flex items-center space-x-2.5">
            <div className="p-1.5 bg-emerald-100 text-emerald-700 rounded-lg">
              <ArrowUpRight className="w-5 h-5" />
            </div>
            <h3 className="text-lg font-bold text-slate-900">Issue Book</h3>
          </div>
          <button
            onClick={onClose}
            className="text-slate-400 hover:text-slate-600 p-1.5 rounded-lg hover:bg-slate-200/60 transition-colors"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        <form onSubmit={handleSubmit} className="p-6 space-y-4">
          {error && (
            <div className="p-3 bg-red-50 border border-red-200 text-red-700 text-sm rounded-lg">
              {error}
            </div>
          )}

          <div className="p-4 bg-slate-50 rounded-xl border border-slate-200">
            <p className="text-xs uppercase font-bold text-slate-400 tracking-wider mb-1">
              Selected Book
            </p>
            <h4 className="text-base font-bold text-slate-900">{book.title}</h4>
            <p className="text-xs text-slate-500 mt-0.5">
              ID: <span className="font-mono font-semibold">{book.id}</span> &bull; Author: {book.author}
            </p>
          </div>

          <div>
            <label className="block text-xs font-bold uppercase tracking-wider text-slate-700 mb-1.5">
              Borrower Name / Member ID <span className="text-red-500">*</span>
            </label>
            <input
              type="text"
              required
              autoFocus
              value={borrowerName}
              onChange={(e) => setBorrowerName(e.target.value)}
              placeholder="e.g. Alice Smith or University ID"
              className="w-full px-3.5 py-2.5 border border-slate-300 rounded-lg text-sm focus:outline-none focus:ring-2 focus:ring-emerald-500 focus:border-transparent"
            />
          </div>

          <div className="pt-4 flex items-center justify-end space-x-3 border-t border-slate-200">
            <button
              type="button"
              onClick={onClose}
              disabled={isSubmitting}
              className="px-4 py-2 text-sm font-medium text-slate-700 hover:bg-slate-100 rounded-lg transition-colors"
            >
              Cancel
            </button>
            <button
              type="submit"
              disabled={isSubmitting}
              className="px-5 py-2 text-sm font-medium text-white bg-emerald-600 hover:bg-emerald-700 rounded-lg transition-colors shadow-sm disabled:opacity-50"
            >
              {isSubmitting ? 'Issuing...' : 'Confirm Issue'}
            </button>
          </div>
        </form>
      </div>
    </div>
  );
};

