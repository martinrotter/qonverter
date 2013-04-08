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

#ifndef CONSTANTSMODEL_H
#define CONSTANTSMODEL_H

#include <QAbstractListModel>
#include <QStringList>

#include "memoryplace.h"


class Calculator;

// This model is used for data abstraction for
// calculator variables and constants.
class ConstantsModel : public QAbstractListModel {
    Q_OBJECT

  public:
    enum QueryType {
      NAME = 0,
      VALUE = 1,
      VALUE_TYPE = 2,
      DESCRIPTION = 3,
      ENTITY_TYPE = 4,
      RAW_VALUE = 5
    };

    // Constructor.
    explicit ConstantsModel(QObject *parent = 0);

    // Overloads from QAbstractListModel.
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;

  private:
    // Initialised to singleton calculator, no need to free from memory here.
    Calculator *m_calculator;

    // Holds titles of columns.
    QStringList m_headerData;
};

#endif // CONSTANTSMODEL_H
