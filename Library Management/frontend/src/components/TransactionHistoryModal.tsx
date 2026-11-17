import React from 'react';
import { X, History, ArrowUpRight, ArrowDownLeft } from 'lucide-react';
import { Transaction } from '../types';

interface TransactionHistoryModalProps {
  isOpen: boolean;
  transactions: Transaction[];
  isLoading: boolean;
  onClose: () => void;
}

export const TransactionHistoryModal: React.FC<TransactionHistoryModalProps> = ({
  isOpen,
  transactions,
  isLoading,
  onClose,
}) => {
  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 z-50 overflow-y-auto bg-slate-900/50 backdrop-blur-sm flex items-center justify-center p-4">
      <div className="bg-white rounded-2xl max-w-4xl w-full shadow-2xl border border-slate-100 overflow-hidden flex flex-col max-h-[85vh] animate-in fade-in zoom-in-95 duration-150">
        <div className="px-6 py-4 border-b border-slate-200 flex items-center justify-between bg-slate-50">
          <div className="flex items-center space-x-2.5">
            <div className="p-1.5 bg-purple-100 text-purple-700 rounded-lg">
              <History className="w-5 h-5" />
            </div>
            <div>
              <h3 className="text-lg font-bold text-slate-900">Transaction Audit Trail</h3>
              <p className="text-xs text-slate-500">
                Complete historical record of book issues and returns
              </p>
            </div>
          </div>
          <button
            onClick={onClose}
            className="text-slate-400 hover:text-slate-600 p-1.5 rounded-lg hover:bg-slate-200/60 transition-colors"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        <div className="flex-1 overflow-y-auto p-6">
          {isLoading ? (
            <div className="py-16 text-center text-slate-500">
              <div className="inline-flex items-center space-x-2">
                <div className="w-5 h-5 border-2 border-purple-600 border-t-transparent rounded-full animate-spin" />
                <span>Loading transaction records...</span>
              </div>
            </div>
          ) : transactions.length === 0 ? (
            <div className="py-16 text-center text-slate-400">
              <p className="text-base font-semibold text-slate-700">No transactions recorded yet</p>
              <p className="text-xs text-slate-500 mt-1">
                Issuing or returning books will automatically generate an audit log.
              </p>
            </div>
          ) : (
            <div className="border border-slate-200 rounded-xl overflow-hidden">
              <table className="min-w-full divide-y divide-slate-200 text-left text-sm">
                <thead className="bg-slate-50 text-slate-500 uppercase tracking-wider text-xs font-semibold">
                  <tr>
                    <th className="px-5 py-3">Txn ID</th>
                    <th className="px-5 py-3">Book ID</th>
                    <th className="px-5 py-3">Book Title</th>
                    <th className="px-5 py-3">Borrower</th>
                    <th className="px-5 py-3">Type</th>
                    <th className="px-5 py-3">Timestamp</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-200 bg-white">
                  {transactions.map((t) => (
                    <tr key={t.transactionId} className="hover:bg-slate-50/80">
                      <td className="px-5 py-3.5 font-mono text-xs font-semibold text-slate-700 whitespace-nowrap">
                        {t.transactionId}
                      </td>
                      <td className="px-5 py-3.5 font-mono text-xs text-slate-600 whitespace-nowrap">
                        {t.bookId}
                      </td>
                      <td className="px-5 py-3.5 font-medium text-slate-900 max-w-xs truncate">
                        {t.bookTitle}
                      </td>
                      <td className="px-5 py-3.5 text-slate-700">{t.borrowerName}</td>
                      <td className="px-5 py-3.5 whitespace-nowrap">
                        {t.type === 'ISSUE' ? (
                          <span className="inline-flex items-center px-2 py-0.5 rounded text-xs font-semibold bg-amber-50 text-amber-700 border border-amber-200">
                            <ArrowUpRight className="w-3 h-3 mr-1" />
                            ISSUE
                          </span>
                        ) : (
                          <span className="inline-flex items-center px-2 py-0.5 rounded text-xs font-semibold bg-emerald-50 text-emerald-700 border border-emerald-200">
                            <ArrowDownLeft className="w-3 h-3 mr-1" />
                            RETURN
                          </span>
                        )}
                      </td>
                      <td className="px-5 py-3.5 text-xs text-slate-500 whitespace-nowrap font-mono">
                        {t.timestamp}
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}
        </div>

        <div className="px-6 py-3.5 bg-slate-50 border-t border-slate-200 flex justify-between items-center text-xs text-slate-500">
          <span>Total Records: {transactions.length}</span>
          <button
            onClick={onClose}
            className="px-4 py-1.5 bg-white border border-slate-300 rounded-lg text-slate-700 font-medium hover:bg-slate-100 transition-colors"
          >
            Close
          </button>
        </div>
      </div>
    </div>
  );
};

