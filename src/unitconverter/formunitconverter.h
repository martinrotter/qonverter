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

#ifndef FORMUNITCONVERTER_H
#define FORMUNITCONVERTER_H

#include <QWidget>

#include "ui_formunitconverter.h"
#include "calculator.h"


namespace Ui {
  class FormUnitConverter;
}

class FormUnitConverter : public QWidget {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit FormUnitConverter(QWidget *parent = 0);
    ~FormUnitConverter();

    // Returns input text box.
    // This is used for setting default focus from the main window.
    LineEdit *getInput();

    static FormUnitConverter *getInstance();

  protected slots:
    void manageCalculatedResult(Calculator::CallerFunction function,
                                const Value &value,
                                const QString &info);
    void manageConvertedResult(const QString &result);
    void requestConversion();

  signals:
    void calculationWanted(Calculator::CallerFunction function, QString expression);
    void conversionWanted(int magnitude_index, int input_unit,
                          int output_unit, const QString &input_value);

  private:
    Ui::FormUnitConverter *m_ui;
    bool m_calculated;

    static FormUnitConverter *s_instance;
};

#endif // FORMUNITCONVERTER_H
