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

#ifndef FORMCALCULATOR_H
#define FORMCALCULATOR_H

#include <QWidget>

#include "ui_formcalculator.h"
#include "calculator.h"


namespace Ui {
  class FormCalculator;
}

class CalculatorHighlighter;
class CalculatorButton;
class FloatingLabel;

class FormCalculator : public QWidget {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit FormCalculator(QWidget *parent = 0);
    ~FormCalculator();

    // Initializes calculator.
    void initialize();

    void setKeypadVisible(bool visible);

    // Returns highlighter associated with calculator.
    CalculatorHighlighter *getHighlighter();

    // Returns static instance of FormCalculator.
    static FormCalculator *getInstance();

    CalculatorInput *getInput();

  protected:
    void resizeEvent(QResizeEvent *event);

  public slots:
    void manageResult(const Calculator::CallerFunction &function,
                      const Value &result,
                      const QString &info);

  protected slots:
    void insertToken();

  signals:
    void calculationWanted(const Calculator::CallerFunction &function,
                           const QString &expression);

  private:
    Ui::FormCalculator *m_ui;
    CalculatorHighlighter *m_highlighter;
    QList<CalculatorButton*> m_buttons;
    FloatingLabel *m_label;

    static FormCalculator *s_instance;
};

#endif // FORMCALCULATOR_H
