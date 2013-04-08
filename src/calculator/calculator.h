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

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QPointer>

#include "mpParser.h"
#include "memoryplace.h"
#include "memorymap.h"
#include "constantsmodel.h"


using namespace mup;

class Calculator : public QObject {
    Q_OBJECT

  public:
    enum CallerFunction {
      CALCULATOR_RESULT,
      CALCULATOR_ONTHEFLY,
      CALCULATOR_ERROR,
      CONVERTER_ERROR,
      CONVERTER_ONTHEFLY,
      EDITOR_ONTHEFLY,
      EDITOR_ERROR
    };

    // Constructors and destructors.
    explicit Calculator(QObject *parent = 0);
    ~Calculator();

    ConstantsModel *getConstantsModel();

    // Returns true if name can be used for new variable or constant.
    bool isNameAllowed(const QString &name);

    // Returns true if function with this name exists.
    // This method wraps bool ParserXBase::IsFunDefined(const string_type &ident).
    bool isFunDefined(const QString &function_name);

  signals:
    // Is emitted if result is calculated:
    // a) result is calculated successfully and is stored in result variables, into is empty.
    // b) error occurred and exception info is stored in info variable, result is empty.
    void resultCalculated(const Calculator::CallerFunction &function,
                          const Value &result = Value(),
                          const QString &info = QString());
    void initialized();

  public slots:
    // Sets up calculator and  creates built-in constants.
    void initialize();

    // Resets built-in constants.
    void loadConstants();

    // Loads stored variables from external file, SQLite.
    void loadMemoryPlaces();

    // Save loaded variables into external file.
    void saveMemoryPlaces();

    // Loads data from implicitly added variables and adds these to
    // variables collection.
    void consolidateMemoryPlaces();

    // Adds special, implicit or explicit variable or constants.
    bool addMemoryPlace(const QString &name,
                        const QString &desc,
                        const MemoryPlace::Type &type,
                        const Value &initial_value = Value());

    // Method edits record in m_variables and in m_parser.
    // Variable is created even if it doesn't exist..
    bool editMemoryPlace(const QString &name,
                         const QString &new_desc,
                         const MemoryPlace::Type &new_type,
                         const Value &new_value);

    // Removes variable by name.
    bool removeMemoryPlace(const QString &name);

    // Changes value of "ans" variable.
    // Note that value of "ans" can be externally obtained
    // by queryVariable("ans", VARCONST_RAW_VALUE).
    void changeAns(const Value &new_value);

    // Returns variable or constant data by name or by index.
    QVariant queryVariable(const QString &name, ConstantsModel::QueryType info_class);
    QVariant queryVariable(int index, ConstantsModel::QueryType info_class);

    // Returns total count of variables/constants.
    int countOfMemoryPlaces();

    // Calculates input expression.
    void calculateExpression(Calculator::CallerFunction function, QString expression);

  private:
    ConstantsModel *m_constantsModel;
    ParserX *m_parser;
    MemoryMap m_memoryPlaces;
};

Q_DECLARE_METATYPE(MemoryPlace::Type)
Q_DECLARE_METATYPE(Value*)

#endif // CALCULATOR_H
