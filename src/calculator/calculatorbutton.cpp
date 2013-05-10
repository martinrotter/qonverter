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

#include <QVariant>

#include "calculatorbutton.h"


CalculatorButton::CalculatorButton(QWidget *parent) : QPushButton(parent) {
}

void CalculatorButton::setOutput(const QString &text) {
  m_output = text;
}

void CalculatorButton::setType(int type) {
  m_type = type;
}

int CalculatorButton::type() const {
  return m_type;
}

QString CalculatorButton::output() const {
  if (property("type").toInt() == (int) CalculatorButton::FUNCTION) {
    // HINT: Each and every "function" button needs to have
    // explicitly set m_output to correct output without "()".
    // For example median function will output "median" + optional "()"
    // which may depend on settings.
    return m_output.isEmpty() == true ? "()" : m_output + "()";
  }
  else {
    return m_output.isEmpty() == true ? text() : m_output;
  }
}
