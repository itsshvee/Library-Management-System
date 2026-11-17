import React, { useEffect } from 'react';
import { CheckCircle2, AlertCircle, X } from 'lucide-react';

export interface ToastMessage {
  id: string;
  type: 'success' | 'error';
  message: string;
}

interface ToastProps {
  toasts: ToastMessage[];
  onDismiss: (id: string) => void;
}

export const Toast: React.FC<ToastProps> = ({ toasts, onDismiss }) => {
  return (
    <div className="fixed bottom-5 right-5 z-50 flex flex-col space-y-2 pointer-events-none">
      {toasts.map((toast) => (
        <ToastItem key={toast.id} toast={toast} onDismiss={onDismiss} />
      ))}
    </div>
  );
};

const ToastItem: React.FC<{ toast: ToastMessage; onDismiss: (id: string) => void }> = ({
  toast,
  onDismiss,
}) => {
  useEffect(() => {
    const timer = setTimeout(() => {
      onDismiss(toast.id);
    }, 4000);
    return () => clearTimeout(timer);
  }, [toast.id, onDismiss]);

  const isSuccess = toast.type === 'success';

  return (
    <div
      className={`pointer-events-auto flex items-center p-4 rounded-xl shadow-lg border text-sm max-w-sm transition-all animate-in slide-in-from-bottom-5 duration-200 ${
        isSuccess
          ? 'bg-emerald-50 border-emerald-200 text-emerald-800'
          : 'bg-red-50 border-red-200 text-red-800'
      }`}
    >
      {isSuccess ? (
        <CheckCircle2 className="w-5 h-5 text-emerald-600 mr-2.5 flex-shrink-0" />
      ) : (
        <AlertCircle className="w-5 h-5 text-red-600 mr-2.5 flex-shrink-0" />
      )}
      <span className="flex-1 font-medium">{toast.message}</span>
      <button
        onClick={() => onDismiss(toast.id)}
        className="ml-3 p-1 rounded-md hover:bg-black/5 transition-colors text-slate-500"
      >
        <X className="w-4 h-4" />
      </button>
    </div>
  );
};

