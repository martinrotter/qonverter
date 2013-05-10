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

#include <QListWidgetItem>

#include "formunitconverter.h"
#include "calculatorwrapper.h"
#include "defs.h"
#include "unitconverter.h"
#include "balloontip.h"


// TODO: Continue refactoring from this point.
FormUnitConverter *FormUnitConverter::s_instance;

FormUnitConverter::FormUnitConverter(QWidget *parent)
  : QWidget(parent), m_ui(new Ui::FormUnitConverter), m_calculated(false) {
  m_ui->setupUi(this);

  // Needed because classical singleton fails due to UI files behavior.
  s_instance = this;

  m_ui->m_txtInput->setIcon(MarkedLineEdit::ERROR);

  // If calculation is wanted by this component, then
  // global calculator should calculate it.
  connect(FormUnitConverter::getInstance(), &FormUnitConverter::calculationWanted,
          CalculatorWrapper::getInstance().getCalculator(), &Calculator::calculateExpression);

  // If global calculator calculated the result, then
  // this component should process the result.
  connect(CalculatorWrapper::getInstance().getCalculator(), &Calculator::resultCalculated,
          FormUnitConverter::getInstance(), &FormUnitConverter::manageCalculatedResult);

  // When text changes, then it should be recalculated.
  connect(m_ui->m_txtInput, &QLineEdit::textChanged, [=] (QString expression) {
    emit calculationWanted(Calculator::CONVERTER_ONTHEFLY, expression);
  });

  // If user swaps the units, then comboboxes should swap their indexes.
  connect(m_ui->m_btnSwapUnits,
          static_cast<void (QAbstractButton::*)(bool)>(&QAbstractButton::clicked),
          [=] () {
    int first_combobox_index = m_ui->m_cmbInputUnit->currentIndex();
    m_ui->m_cmbInputUnit->setCurrentIndex(m_ui->m_cmbOutputUnit->currentIndex());
    m_ui->m_cmbOutputUnit->setCurrentIndex(first_combobox_index);
  });

  // If one of the units is changed, then conversion should repeat.
  connect(m_ui->m_cmbInputUnit, &QComboBox::currentTextChanged, [=] () {
    requestConversion();
  });

  connect(m_ui->m_cmbOutputUnit, &QComboBox::currentTextChanged, [=] () {
    requestConversion();
  });

  connect(m_ui->m_txtInput, &MarkedLineEdit::markIconHovered,
          [=] (const MarkedLineEdit::Status &status) {
    if (status == MarkedLineEdit::ERROR) {
      BalloonTip::showBalloon(m_calculationInformation,
                              mapToGlobal(m_ui->m_txtInput->pos() + QPoint(22, 12)),
                              -1);
    }
    else {
      BalloonTip::showBalloon(tr("Input expression is valid."),
                              mapToGlobal(m_ui->m_txtInput->pos() + QPoint(22, 12)),
                              -1);
    }
  });

  // If we need to convert, then do the conversion.
  connect(FormUnitConverter::getInstance(), &FormUnitConverter::conversionWanted,
          &UnitConverter::getInstance(), &UnitConverter::convert);

  // If result is converted, then process it.
  connect(&UnitConverter::getInstance(), &UnitConverter::converted,
          FormUnitConverter::getInstance(), &FormUnitConverter::manageConvertedResult);

  // User selected another magnitude, clear fields and select it.
  connect(m_ui->m_listMagnitudes, &QListWidget::currentItemChanged,
          [=] (QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current != nullptr) {
      // Obtain names for units.
      QList<QString> rate_names = UnitConverter::getInstance().getUnits(current->data(32).toInt());

      // Load new units.
      m_ui->m_cmbInputUnit->clear();
      m_ui->m_cmbOutputUnit->clear();
      m_ui->m_cmbInputUnit->addItems(rate_names);
      m_ui->m_cmbOutputUnit->addItems(rate_names);
    }
  });

  // Load magnitudes.
  QList<QString> magnitudes = UnitConverter::getInstance().getMagnitudes();
  int i = 0;

  // Each magnitude is assigned unique integer.
  // This integer is used for loading units of that particular mangitude.
  foreach (QString magnitude, magnitudes) {
    QListWidgetItem *item = new QListWidgetItem(magnitude, m_ui->m_listMagnitudes);
    item->setData(32, i++);
  }

  // Keep magnitudes sorted.
  m_ui->m_listMagnitudes->sortItems();
  m_ui->m_listMagnitudes->setCurrentRow(0);
}

void FormUnitConverter::requestConversion() {
  if (m_calculated == true &&
      m_ui->m_cmbInputUnit->currentIndex() >= 0 &&
      m_ui->m_cmbOutputUnit->currentIndex() >= 0) {
    emit conversionWanted(m_ui->m_listMagnitudes->currentItem()->data(32).toInt(),
                          m_ui->m_cmbInputUnit->currentIndex(),
                          m_ui->m_cmbOutputUnit->currentIndex(),
                          m_ui->m_txtCalculatedInput->text());
  }
}

FormUnitConverter::~FormUnitConverter() {
  delete m_ui;
}

FormUnitConverter *FormUnitConverter::getInstance() {
  return s_instance;
}

MarkedLineEdit *FormUnitConverter::getInput() {
  return m_ui->m_txtInput;
}

void FormUnitConverter::manageConvertedResult(const QString &result) {
  m_ui->m_txtConvertedInput->setText(result);
}

void FormUnitConverter::manageCalculatedResult(Calculator::CallerFunction function,
                                               const Value &value,
                                               const QString &info) {
  //Q_UNUSED(info);

  switch (function) {
    // User entered valid expression which can be converted.
    case Calculator::CONVERTER_ONTHEFLY: {
      QString textual_value = QString::fromStdWString(value.ToString());

      m_calculated = true;
      m_ui->m_txtCalculatedInput->setText(textual_value);

      emit conversionWanted(m_ui->m_listMagnitudes->currentItem()->data(32).toInt(),
                            m_ui->m_cmbInputUnit->currentIndex(),
                            m_ui->m_cmbOutputUnit->currentIndex(),
                            textual_value);

      BalloonTip::hideBalloon();
      m_ui->m_txtInput->setIcon(MarkedLineEdit::OK);
      break;
    }
      // Error - no conversions here.
    case Calculator::CONVERTER_ERROR: {
      m_calculated = false;
      m_calculationInformation = info;
      m_ui->m_txtCalculatedInput->clear();
      m_ui->m_txtConvertedInput->clear();

      BalloonTip::showBalloon(info,
                              mapToGlobal(m_ui->m_txtInput->pos() + QPoint(22, 12)),
                              -1);
      m_ui->m_txtInput->setIcon(MarkedLineEdit::ERROR);
      break;
    }
    default:
      break;
  }
}
