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

#ifndef CONSTANTSFILTERMODEL_H
#define CONSTANTSFILTERMODEL_H

#include <QSortFilterProxyModel>


class ConstantsModel;

class ConstantsFilterModel : public QSortFilterProxyModel {
    Q_OBJECT

  public:
    enum FilterMemoryPlaceType {
      CONSTANTS = 0,
      IMPLICIT_VARIABLES = 1,
      EXPLICIT_VARIABLES = 2,
      SPECIAL_VARIABLES = 3,
      FUNCTIONS = 4,
      VARIABLES = 5,
      ALL = 6
    };

    explicit ConstantsFilterModel(QObject *parent = 0,
                                  const FilterMemoryPlaceType &filter_type = ConstantsFilterModel::ALL);

    void setSourceModel(QAbstractItemModel *source_model);

  public slots:
    void setFilterMemoryPlaceType(const FilterMemoryPlaceType &type);

  protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

  private:
    FilterMemoryPlaceType m_filterType;
    ConstantsModel *m_sourceModel;
};

#endif // CONSTANTSFILTERMODEL_H
