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

#ifndef CALCULATORWRAPPER_H
#define CALCULATORWRAPPER_H

#include <QObject>


class Calculator;
class QThread;

class CalculatorWrapper : public QObject {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit CalculatorWrapper(QObject *parent = 0);
    ~CalculatorWrapper();

    // Returns calculator associated with this thread.
    Calculator *getCalculator();

    // Returns singleton associated with this thread.
    static CalculatorWrapper &getInstance();

  private:
    Calculator *m_calculator;
    QThread *m_thread;

    static QScopedPointer<CalculatorWrapper> s_instance;
};

#endif // CALCULATORWRAPPER_H
