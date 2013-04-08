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
#include <QDebug>
#include <QVariant>

#include "formcalculator.h"
#include "calculatorhighlighter.h"
#include "defs.h"
#include "settings.h"
#include "calculatorbutton.h"
#include "calculatorwrapper.h"
#include "floatinglabel.h"


FormCalculator *FormCalculator::s_instance;

FormCalculator::FormCalculator(QWidget *parent) : QWidget(parent), m_ui(new Ui::FormCalculator) {
  m_ui->setupUi(this);

  // Needed because classical singleton fails due to UI files behavior.
  s_instance = this;

  // Set properties for all buttons. This is used for functions and for skins.

  // Fill in buttons and make them react to clicks.
  // Make sure buttons are structured correctly in this array.
  QList<CalculatorButton*> but_numbers;
  but_numbers << m_ui->m_btnOne << m_ui->m_btnTwo << m_ui->m_btnThree <<
                 m_ui->m_btnFour << m_ui->m_btnFive << m_ui->m_btnSix <<
                 m_ui->m_btnSeven << m_ui->m_btnEight << m_ui->m_btnNine <<
                 m_ui->m_btnZero << m_ui->m_btnDot;

  QList<CalculatorButton*> but_operators;
  but_operators << m_ui->m_btnPlus << m_ui->m_btnMinus << m_ui->m_btnTimes <<
                   m_ui->m_btnDivide  << m_ui->m_btnFact << m_ui->m_btnSqrt <<
                   m_ui->m_btnPow;

  QList<CalculatorButton*> but_solvers;
  but_solvers << m_ui->m_btnResult;

  QList<CalculatorButton*> but_functions;
  but_functions << m_ui->m_btnParents << m_ui->m_btnLog << m_ui->m_btnMedian <<
                   m_ui->m_btnSin << m_ui->m_btnCos << m_ui->m_btnTan <<
                   m_ui->m_btnLn << m_ui->m_btnExp << m_ui->m_btnMax <<
                   m_ui->m_btnMin << m_ui->m_btnSum << m_ui->m_btnAbs <<
                   m_ui->m_btnReal << m_ui->m_btnImag << m_ui->m_btnArg <<
                   m_ui->m_btnNorm;

  QList<CalculatorButton*> but_compare;
  but_compare << m_ui->m_btnEqual << m_ui->m_btnLess << m_ui->m_btnMore <<
                 m_ui->m_btnAnd << m_ui->m_btnOr;

  QList<CalculatorButton*> but_bit;
  but_bit << m_ui->m_btnBitOr << m_ui->m_btnBitAnd << m_ui->m_btnShiftLeft <<
             m_ui->m_btnShiftRight;

  QList<CalculatorButton*> but_control;
  but_control << m_ui->m_btnClear << m_ui->m_btnMemoryClear << m_ui->m_btnMemorySub <<
                 m_ui->m_btnMemoryAdd << m_ui->m_btnMemorySet;

  foreach (CalculatorButton *btn, but_numbers) {
    btn->setProperty("type", (int) CalculatorButton::NUMBER);
  }

  foreach (CalculatorButton *btn, but_operators) {
    btn->setProperty("type", (int) CalculatorButton::OPERATOR);
  }

  foreach (CalculatorButton *btn, but_solvers) {
    btn->setProperty("type", (int) CalculatorButton::SOLVER);
  }

  foreach (CalculatorButton *btn, but_functions) {
    btn->setProperty("type", (int) CalculatorButton::FUNCTION);
  }

  foreach (CalculatorButton *btn, but_compare) {
    btn->setProperty("type", (int) CalculatorButton::COMPARE);
  }

  foreach (CalculatorButton *btn, but_bit) {
    btn->setProperty("type", (int) CalculatorButton::BIT);
  }

  foreach (CalculatorButton *btn, but_control) {
    btn->setProperty("type", (int) CalculatorButton::CONTROL);
  }

  m_buttons << but_numbers << but_operators << but_solvers <<
               but_functions << but_compare << but_bit;

  // Set outputs of specific buttons.
  m_ui->m_btnBitAnd->setOutput("&");
  m_ui->m_btnParents->setOutput("");
  m_ui->m_btnLog->setOutput("log");
  m_ui->m_btnSin->setOutput("sin");
  m_ui->m_btnCos->setOutput("cos");
  m_ui->m_btnTan->setOutput("tan");
  m_ui->m_btnLn->setOutput("ln");
  m_ui->m_btnAbs->setOutput("abs");
  m_ui->m_btnMax->setOutput("max");
  m_ui->m_btnMin->setOutput("min");
  m_ui->m_btnSum->setOutput("sum");
  m_ui->m_btnMedian->setOutput("median");
  m_ui->m_btnReal->setOutput("real");
  m_ui->m_btnImag->setOutput("imag");
  m_ui->m_btnArg->setOutput("arg");
  m_ui->m_btnNorm->setOutput("norm");
  m_ui->m_btnExp->setOutput("exp");

  // Prepare gui of calculator widget.
  m_highlighter = new CalculatorHighlighter(m_ui->m_input);
  m_label = new FloatingLabel(m_ui->m_input);

  // User hit ENTER button to calculate the result.
  connect(m_ui->m_input, &CalculatorInput::submitted, [=] (QString expression) {
    qDebug().nospace() << "Asking for calculation from thread " << QThread::currentThreadId() << ".";
    emit calculationWanted(Calculator::CALCULATOR_RESULT, expression);
  });

  // User is changing input epression via keypad or keyboard.
  connect(m_ui->m_input, &CalculatorInput::textEdited, [=] (QString expression) {
    if (Settings::value(APP_CFG_CALC, "on-the-fly", true).toBool() == true) {
      qDebug().nospace() << "Asking for calculation from thread " << QThread::currentThreadId() << ".";
      emit calculationWanted(Calculator::CALCULATOR_ONTHEFLY, expression);
    }
  });

  // User pushes any button on the keypad (except ENTER).
  foreach (CalculatorButton *button, m_buttons) {
    connect(button, &CalculatorButton::clicked, this, &FormCalculator::insertToken);
  }

  // Functionality for "MS" button.
  connect(m_ui->m_btnMemorySet, &CalculatorButton::clicked, [=] () {
    m_ui->m_input->setFocus(Qt::TabFocusReason);
    emit calculationWanted(Calculator::CALCULATOR_ONTHEFLY,
                           QString("m=%1").arg(m_ui->m_input->toPlainText()));
  });

  // Functionality for "MC" button.
  connect(m_ui->m_btnMemoryClear, &CalculatorButton::clicked, [=] () {
    m_ui->m_input->setFocus(Qt::TabFocusReason);
    emit calculationWanted(Calculator::CALCULATOR_ONTHEFLY, "m=0");
  });

  // Functionality for "M-" button.
  connect(m_ui->m_btnMemorySub, &CalculatorButton::clicked, [=] () {
    m_ui->m_input->setFocus(Qt::TabFocusReason);
    emit calculationWanted(Calculator::CALCULATOR_ONTHEFLY,
                           QString("m-=%1").arg(m_ui->m_input->toPlainText()));
  });

  // Functionality for "M+" button.
  connect(m_ui->m_btnMemoryAdd, &CalculatorButton::clicked, [=] () {
    m_ui->m_input->setFocus(Qt::TabFocusReason);
    emit calculationWanted(Calculator::CALCULATOR_ONTHEFLY,
                           QString("m+=%1").arg(m_ui->m_input->toPlainText()));
  });

  // Functionality for "C" button.
  connect(m_ui->m_btnClear, &CalculatorButton::clicked, [=] () {
    m_ui->m_input->clear();
    m_ui->m_input->setFocus(Qt::TabFocusReason);
  });

  // Connect global calculator to the calculator widget.
  connect(FormCalculator::getInstance(), &FormCalculator::calculationWanted,
          CalculatorWrapper::getInstance().getCalculator(), &Calculator::calculateExpression);

  connect(CalculatorWrapper::getInstance().getCalculator(), &Calculator::resultCalculated,
          FormCalculator::getInstance(), &FormCalculator::manageResult);

  // Initialize settings of the form.
  initialize();
}

void FormCalculator::insertToken() {
  CalculatorButton *clicked_button = qobject_cast<CalculatorButton*>(sender());

  if (clicked_button->property("type").toInt() == (int) CalculatorButton::SOLVER) {
    qDebug().nospace() << "Asking for calculation from thread " << QThread::currentThreadId() << ".";
    emit calculationWanted(Calculator::CALCULATOR_RESULT, m_ui->m_input->toPlainText());
  }
  else {
    m_ui->m_input->insertPlainText(clicked_button->output());

    if (clicked_button->property("type").toInt() == (int) CalculatorButton::FUNCTION) {
      m_ui->m_input->moveCursor(QTextCursor::Left,
                                QTextCursor::MoveAnchor);
    }
  }

  // It's good to switch focus to input text box so that user can optionally continue
  // by inputting text via physical keyboard.
  m_ui->m_input->setFocus(Qt::TabFocusReason);

  // On-the-fly function. It's needed to explicitly call here
  // because textEdited function catches just by-keyboard editings.
  if (Settings::value(APP_CFG_CALC, "on-the-fly", true).toBool()) {
    qDebug().nospace() << "Asking for calculation from thread " << QThread::currentThreadId() << ".";
    emit calculationWanted(Calculator::CALCULATOR_ONTHEFLY, m_ui->m_input->toPlainText());
  }
}

CalculatorInput *FormCalculator::getInput() {
  return m_ui->m_input;
}

FormCalculator::~FormCalculator() {
  delete m_ui;
  delete m_highlighter;
  delete m_label;
}

CalculatorHighlighter *FormCalculator::getHighlighter() {
  return m_highlighter;
}

void FormCalculator::initialize() {
  m_ui->m_input->initialize();
  m_label->initialize();
  setKeypadVisible(Settings::value(APP_CFG_CALC, "keypad_visible", true).toBool());
}

void FormCalculator::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event)

  m_label->adjust();
}

void FormCalculator::manageResult(Calculator::CallerFunction function,
                                  const Value &result,
                                  const QString &info) {
  switch (function) {
    case Calculator::CALCULATOR_ERROR:
      m_ui->m_input->setFocus(Qt::TabFocusReason);
      m_label->showText(info,
                        Settings::value(APP_CFG_CALC,
                                        "length_error",
                                        3000).toInt());
      break;
    case Calculator::CALCULATOR_ONTHEFLY:
      m_label->showText(tr("<b>Result: </b>") + QString::fromStdWString(result.ToString()),
                        Settings::value(APP_CFG_CALC,
                                        "length_fly",
                                        1500).toInt());
      break;
    case Calculator::CALCULATOR_RESULT:
      m_ui->m_input->setPlainText(QString::fromStdWString(result.ToString()));
      if (Settings::value(APP_CFG_CALC, "select_result", true).toBool()) {
        m_ui->m_input->selectAll();
      }
      else {
        m_ui->m_input->moveCursor(QTextCursor::End);
      }
      m_ui->m_input->setFocus(Qt::TabFocusReason);
      // TODO: Tady zkusit resetovat popup z kompleteru
      // pro opraveni chyby s chybejicima polozkama
      break;
    default:
      break;
  }
}

void FormCalculator::setKeypadVisible(bool visible) {
  m_ui->m_keypad->setVisible(visible);
  m_label->adjust();
}

FormCalculator *FormCalculator::getInstance() {
  return s_instance;
}
