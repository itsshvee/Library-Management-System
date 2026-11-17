import React from 'react';
import { Book, CheckCircle2, AlertCircle, ArrowLeftRight } from 'lucide-react';
import { DashboardStats as StatsType } from '../types';

interface DashboardStatsProps {
  stats: StatsType | null;
  isLoading: boolean;
  activeFilter: string;
  onFilterChange: (filter: 'all' | 'available' | 'issued') => void;
}

export const DashboardStats: React.FC<DashboardStatsProps> = ({
  stats,
  isLoading,
  activeFilter,
  onFilterChange,
}) => {
  const cards = [
    {
      label: 'Total Registered Books',
      value: stats ? stats.totalBooks : 0,
      icon: Book,
      color: 'blue',
      filter: 'all' as const,
      bgClass: 'bg-blue-50 text-blue-600',
      activeRing: activeFilter === 'all' ? 'ring-2 ring-blue-500 shadow-md' : '',
    },
    {
      label: 'Available Books',
      value: stats ? stats.availableBooks : 0,
      icon: CheckCircle2,
      color: 'emerald',
      filter: 'available' as const,
      bgClass: 'bg-emerald-50 text-emerald-600',
      activeRing: activeFilter === 'available' ? 'ring-2 ring-emerald-500 shadow-md' : '',
    },
    {
      label: 'Currently Issued',
      value: stats ? stats.issuedBooks : 0,
      icon: AlertCircle,
      color: 'amber',
      filter: 'issued' as const,
      bgClass: 'bg-amber-50 text-amber-600',
      activeRing: activeFilter === 'issued' ? 'ring-2 ring-amber-500 shadow-md' : '',
    },
    {
      label: 'Total Transactions',
      value: stats ? stats.totalTransactions : 0,
      icon: ArrowLeftRight,
      color: 'purple',
      filter: null,
      bgClass: 'bg-purple-50 text-purple-600',
      activeRing: '',
    },
  ];

  return (
    <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6 mb-8">
      {cards.map((card, idx) => {
        const Icon = card.icon;
        const isClickable = card.filter !== null;

        return (
          <div
            key={idx}
            onClick={() => isClickable && onFilterChange(card.filter)}
            className={`bg-white rounded-xl border border-slate-200 p-5 transition-all duration-200 ${
              isClickable ? 'cursor-pointer hover:border-slate-300 hover:shadow-sm' : ''
            } ${card.activeRing}`}
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-xs font-semibold text-slate-500 uppercase tracking-wider mb-1">
                  {card.label}
                </p>
                <div className="text-2xl sm:text-3xl font-extrabold text-slate-900">
                  {isLoading ? (
                    <span className="inline-block w-8 h-8 bg-slate-200 rounded animate-pulse" />
                  ) : (
                    card.value
                  )}
                </div>
              </div>
              <div className={`p-3 rounded-xl ${card.bgClass}`}>
                <Icon className="w-6 h-6" />
              </div>
            </div>
            {isClickable && (
              <p className="text-xs text-slate-400 mt-3 font-medium">
                Click to filter list &rarr;
              </p>
            )}
          </div>
        );
      })}
    </div>
  );
};

