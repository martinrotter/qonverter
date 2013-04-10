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

#include <QStringList>
#include <QBrush>

#include "constantsmodel.h"
#include "calculator.h"
#include "calculatorwrapper.h"
#include "constantsmodel.h"


ConstantsModel::ConstantsModel(QObject *parent)
  : QAbstractListModel(parent),
    m_calculator(CalculatorWrapper::getInstance().getCalculator()) {
  m_headerData << tr("Name") << tr("Value") << tr("Type") << tr("Description");
}

int ConstantsModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)

  return m_calculator->countOfMemoryPlaces();
}

int ConstantsModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)

  return m_headerData.size();
}

QVariant ConstantsModel::data(const QModelIndex &index, int role) const {
  switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
      switch (index.column()) {
        case (int) ConstantsModel::DESCRIPTION: {
          QString description = m_calculator->queryVariable(index.row(),
                                                            ConstantsModel::DESCRIPTION).toString();
          return description.isEmpty() == true ? tr("no description") : description;
        }
        case (int) ConstantsModel::VALUE_TYPE : {
          QChar type = m_calculator->queryVariable(index.row(),
                                                   static_cast<ConstantsModel::QueryType>(index.column())).toChar();
          MemoryPlace::Type variable_type = static_cast<MemoryPlace::Type>(m_calculator->queryVariable(index.row(),
                                                                                                       ConstantsModel::ENTITY_TYPE).toInt());

          if (variable_type == MemoryPlace::FUNCTION) {
            return "-";
          }

          switch (type.toLatin1()) {
            case 'i':
              return "INTEGER";
            case 'f':
              return "FLOAT";
            case 'b':
              return "BOOLEAN";
            case 's':
              return "STRING";
            case 'c':
              return "COMPLEX";
            default:
              return "VOID";
          }
        }
        case (int) ConstantsModel::VALUE: {
          MemoryPlace::Type variable_type = static_cast<MemoryPlace::Type>(m_calculator->queryVariable(index.row(),
                                                                                                       ConstantsModel::ENTITY_TYPE).toInt());

          if (variable_type == MemoryPlace::FUNCTION) {
            return "-";
          }
          else {
            m_calculator->queryVariable(index.row(),
                                        static_cast<ConstantsModel::QueryType>(index.column()));
          }
        }
        default:
          return m_calculator->queryVariable(index.row(),
                                             static_cast<ConstantsModel::QueryType>(index.column()));
      }
    case Qt::BackgroundRole: {
      MemoryPlace::Type type = static_cast<MemoryPlace::Type>(m_calculator->queryVariable(index.row(),
                                                                                          ConstantsModel::ENTITY_TYPE).toInt());
      switch (type) {
        case MemoryPlace::FUNCTION:
          return QBrush(QColor(0, 0, 255, 20));
        case MemoryPlace::CONSTANT:
          return QBrush(QColor(255, 0, 0, 20));
        case MemoryPlace::EXPLICIT_VARIABLE:
        case MemoryPlace::IMPLICIT_VARIABLE:
          return QBrush(QColor(0, 255, 0, 20));
        case MemoryPlace::SPECIAL_VARIABLE:
          return QBrush(QColor(255, 196, 0, 20));
        default:
          return QVariant();
      }
    }
    case Qt::TextAlignmentRole:
      return Qt::AlignLeft;
    case Qt::UserRole:
      // This role is used to return raw information.
      switch (index.column()) {
        case (int) ConstantsModel::ENTITY_TYPE:
          return static_cast<MemoryPlace::Type>(m_calculator->queryVariable(index.row(),
                                                                            ConstantsModel::ENTITY_TYPE).toInt());
        case (int) ConstantsModel::DESCRIPTION:
          return m_calculator->queryVariable(index.row(), ConstantsModel::DESCRIPTION);
        default:
          return QVariant();
      }
    default:
      return QVariant();
  }
}

QVariant ConstantsModel::headerData(int section, Qt::Orientation orientation, int role) const {
  Q_UNUSED(orientation)

  if (section >= m_headerData.size() || section < 0) {
    return QVariant();
  }

  switch (role) {
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
      return m_headerData.at(section);
    default:
      return QVariant();
  }
}

QModelIndex ConstantsModel::index(int row, int column, const QModelIndex &parent) const {
  Q_UNUSED(parent)

  return createIndex(row, column);
}

