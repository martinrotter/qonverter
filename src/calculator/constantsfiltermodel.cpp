/*
 This file is part of Qonverter.

 Qonverter is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Qonverter is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Qonverter.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2012 - 2013 Martin Rotter
*/

#include "calculator.h"
#include "constantsfiltermodel.h"
#include "constantsmodel.h"


ConstantsFilterModel::ConstantsFilterModel(QObject *parent,
                                           const FilterMemoryPlaceType &filter_type)
  : QSortFilterProxyModel(parent), m_filterType(filter_type) {
}

bool ConstantsFilterModel::filterAcceptsRow(int source_row,
                                            const QModelIndex &source_parent) const {
  MemoryPlace::Type var_type = static_cast<MemoryPlace::Type>(m_sourceModel->data(m_sourceModel->index(source_row,
                                                                                                       (int) ConstantsModel::ENTITY_TYPE),
                                                                                  Qt::UserRole).toInt());
  return
      (m_filterType == ALL ||
      (int) var_type == (int) m_filterType ||
      (m_filterType == VARIABLES &&
       (int) var_type >= IMPLICIT_VARIABLES &&
       (int) var_type <= SPECIAL_VARIABLES)) &&
      // For compatibility with class filtering ways.
      QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

void ConstantsFilterModel::setSourceModel(QAbstractItemModel *source_model) {
  m_sourceModel = (ConstantsModel*) source_model;
  QSortFilterProxyModel::setSourceModel(source_model);
}

void ConstantsFilterModel::setFilterMemoryPlaceType(const FilterMemoryPlaceType &type) {
  m_filterType = type;
  invalidate();
}

