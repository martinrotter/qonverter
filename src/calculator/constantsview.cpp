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

#include <QHeaderView>
#include <QMessageBox>
#include <QModelIndex>

#include "constantsview.h"
#include "constantsmodel.h"
#include "calculatorwrapper.h"
#include "calculator.h"
#include "constantsfiltermodel.h"


ConstantsView::ConstantsView(QWidget *parent)
  : QTreeView(parent),
    m_model(CalculatorWrapper::getInstance().getCalculator()->getConstantsModel()),
    m_filterModel(new ConstantsFilterModel(this)) {
  // Setup model chain.
  m_filterModel->setSourceModel(m_model);
  setModel(m_filterModel);

  // Enable sorting and do initial sort.
  setSortingEnabled(true);
  sortByColumn(0, Qt::AscendingOrder);

  // Selection mode.
  setSelectionMode(QAbstractItemView::SingleSelection);

  // Tweak columns.
  header()->setSectionsMovable(false);
  header()->setSectionsClickable(true);
  header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(3, QHeaderView::Stretch);
}

ConstantsView::~ConstantsView() {
}

void ConstantsView::reset() {
  // Get currently selected row and the actual row count.
  QModelIndex new_marked_index = currentIndex();

  // Make sure that filtering is invalidated.
  m_filterModel->invalidate();

  // Get current row count, which IS CORRECT due to invalidation.
  int row_count = m_filterModel->rowCount(QModelIndex());

  // Reset view to initial state.
  // This completely refreshes displayed data about memory places.
  QTreeView::reset();

  // View displays at least one row.
  if (row_count > 0) {
    // Index points to non-existing item. Mark last item.
    if (new_marked_index.row() >= row_count || new_marked_index.row() < 0) {
      new_marked_index = m_filterModel->index(row_count - 1, 0);
    }
    // Index points to correct item. Mark it.
    else {
      new_marked_index = m_filterModel->index(new_marked_index.row(), 0);
    }
    setCurrentIndex(new_marked_index);
  }

  emit currentItemChanged(new_marked_index);
}

void ConstantsView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
  QTreeView::currentChanged(current, previous);

  emit currentItemChanged(current);
}
