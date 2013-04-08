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

#ifndef CALCULATORBUTTON_H
#define CALCULATORBUTTON_H

#include <QPushButton>


class CalculatorButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType)

  public:
    enum Type {
      NUMBER	= 0,
      OPERATOR	= 1,
      FUNCTION	= 2,
      SOLVER	= 3,
      COMPARE	= 4,
      CONTROL	= 5,
      BIT		= 6
    };

    // Constructor.
    explicit CalculatorButton(QWidget *parent = 0);

    // Sets/gets the type of button.
    void setType(int type);
    int type() const;

    // Sets custom output of the button.
    void setOutput(const QString &text);

    // Returns textual output to be printed out after the push of this button.
    // Output may be the same as the text of this button.
    QString output();

  private:
    // Type of the button.
    int m_type;

    // Some buttons need to output more sophisticated strings.
    QString m_output;
};

#endif // CALCULATORBUTTON_H
