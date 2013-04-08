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

#include <QThread>

#include "calculator.h"
#include "calculatorwrapper.h"


QScopedPointer<CalculatorWrapper> CalculatorWrapper::s_instance;

CalculatorWrapper::CalculatorWrapper(QObject *parent) : QObject(parent) {
  // Create calculator.
  m_calculator = new Calculator();

  // Create separate thread for calculator.
  m_thread = new QThread();

  // Prepare calculator for usage in separate thread.
  m_calculator->moveToThread(m_thread);

  // Connect thread to calculator.
  connect(m_thread, &QThread::started, m_calculator, &Calculator::initialize);
  connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
}

CalculatorWrapper::~CalculatorWrapper() {
  qDebug("Deleting calculator wrapper.");

  m_thread->quit();
  m_thread->wait(1000);

  delete m_calculator;
}

Calculator *CalculatorWrapper::getCalculator() {
  return m_calculator;
}

CalculatorWrapper &CalculatorWrapper::getInstance() {
  if (s_instance.isNull() == true) {
    s_instance.reset(new CalculatorWrapper());
    s_instance.data()->m_thread->start();
  }

  return *s_instance;
}
