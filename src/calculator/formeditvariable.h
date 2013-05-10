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

#ifndef FORMEDITVARIABLE_H
#define FORMEDITVARIABLE_H

#include <QDialog>

#include "ui_formeditvariable.h"
#include "calculator.h"


namespace Ui {
  class FormEditVariable;
}

class FormEditVariable : public QDialog {
    Q_OBJECT
    
  public:
    // Constructor and destructors.
    explicit FormEditVariable(QWidget *parent = 0);
    ~FormEditVariable();

    // Displays dialog for adding memory places.
    int execAdd();

    // Displays dialog for editing existing memory places.
    int execEdit(const QString &name, const QString& value,
                 const QString &description, const QString &type,
                 const MemoryPlace::Type &var_type);
    
  signals:
    // Emitted if dialog needs to verify type of result
    // of input formula.
    void calculationWanted(const Calculator::CallerFunction &function,
                           const QString &expression);

  protected slots:
    // Displays information about proposed expression.
    void manageResult(const Calculator::CallerFunction &function, const Value &result);

    // Checks if proposed name can be used for memory place.
    void checkName(const QString &name);

  private:
    Ui::FormEditVariable *m_ui;
    QMap<QString, int> m_mappedTypes;
    QPalette m_defaultPalette;
    QPalette m_errorPalette;
    Value m_convertedValue;
    QString m_originalName;

    bool m_correctName;
    bool m_correctValue;
};

#endif // FORMEDITVARIABLE_H
