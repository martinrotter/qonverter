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

#include <QPushButton>

#include "messagebox.h"
#include "formeditvariable.h"
#include "calculatorwrapper.h"
#include "calculator.h"


FormEditVariable::FormEditVariable(QWidget *parent)
  : QDialog(parent), m_ui(new Ui::FormEditVariable) {
  m_ui->setupUi(this);

  // Set flags.
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);

  // Set palletes.
  m_defaultPalette = m_ui->m_txtCalculatedValue->palette();
  m_errorPalette = m_defaultPalette;
  m_errorPalette.setColor(QPalette::Base, QColor(255, 0, 0, 20));
  m_defaultPalette.setColor(QPalette::Base, QColor(0, 255, 0, 20));

  // Set mappings.
  m_mappedTypes["INTEGER"] = m_mappedTypes["i"] = 0;
  m_mappedTypes["FLOAT"] = m_mappedTypes["f"] = 1;
  m_mappedTypes["BOOLEAN"] = m_mappedTypes["b"] = 2;
  m_mappedTypes["STRING"] = m_mappedTypes["s"] = 3;
  m_mappedTypes["COMPLEX"] = m_mappedTypes["c"] = 4;
  m_mappedTypes["VOID"] = m_mappedTypes["v"] = 5;

  // Connect global calculator evaluator to this widget.
  // Calculator does checking if input string could be value of given type.
  connect(this, &FormEditVariable::calculationWanted,
          CalculatorWrapper::getInstance().getCalculator(),
          &Calculator::calculateExpression);

  connect(CalculatorWrapper::getInstance().getCalculator(),
          &Calculator::resultCalculated,
          this, &FormEditVariable::manageResult);

  // Make sure that final value is recalculated when input value changes
  // or when another type is selected.
  connect(m_ui->m_cmbType,
          static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
          [=] () {
    emit calculationWanted(Calculator::EDITOR_ONTHEFLY, m_ui->m_txtValue->text());
  });

  connect(m_ui->m_txtValue, &QLineEdit::textChanged, [=] (const QString &text) {
    emit calculationWanted(Calculator::EDITOR_ONTHEFLY, text);
  });

  connect(m_ui->m_txtName, &QLineEdit::textChanged, this, &FormEditVariable::checkName);
}

FormEditVariable::~FormEditVariable() {
  delete m_ui;
}

void FormEditVariable::checkName(const QString &name) {
  if (CalculatorWrapper::getInstance().getCalculator()->isNameAllowed(name) == true ||
      (m_originalName.isEmpty() == false && name == m_originalName)) {
    // Name can be used for memory place.
    m_ui->m_txtName->setIcon(MarkedLineEdit::OK);
    m_correctName = true;
  }
  else {
    // Name cannot be used for memory place.
    m_ui->m_txtName->setIcon(MarkedLineEdit::ERROR);
    m_correctName = false;
  }

  // Enable or disable "OK" button according to input data.
  m_ui->m_btnBox->button(QDialogButtonBox::Ok)->setEnabled(m_correctName && m_correctValue);
}

void FormEditVariable::manageResult(Calculator::CallerFunction function,
                                    const Value &result) {
  Q_UNUSED(function);

  m_correctValue = m_mappedTypes[QString(result.GetType())] ==
                   m_ui->m_cmbType->currentIndex();
  m_convertedValue = result;
  m_ui->m_txtCalculatedValue->setText(QString::fromStdWString(result.ToString()));
  m_ui->m_txtValue->setIcon(m_correctValue ?
                              MarkedLineEdit::OK :
                              MarkedLineEdit::ERROR);
  m_ui->m_lblInfo->setText(m_correctValue ?
                             tr("Result of expression is of type '%1' and can be used as value for variable '%2'.").arg(m_ui->m_cmbType->currentText(),
                                                                                                                        m_ui->m_txtName->text()):
                             tr("Result of expression is not of type '%1'. Make sure correct type is selected. If you select 'VOID' type, then make sure your expression evaluates to 'VOID'. Empty expression does that.").arg(m_ui->m_cmbType->currentText()));
  m_ui->m_btnBox->button(QDialogButtonBox::Ok)->setEnabled(m_correctName && m_correctValue);
}

int FormEditVariable::execAdd() {
  // Set initial focus and controls
  m_ui->m_txtName->setText("");
  m_ui->m_cmbType->setCurrentIndex(0);
  m_ui->m_txtName->setFocus(Qt::TabFocusReason);
  m_ui->m_btnBox->button(QDialogButtonBox::Ok)->setEnabled(false);

  connect(m_ui->m_btnBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, [=] () {
    if (CalculatorWrapper::getInstance().getCalculator()->addMemoryPlace(m_ui->m_txtName->text(),
                                                                         m_ui->m_txtDescription->text(),
                                                                         MemoryPlace::EXPLICIT_VARIABLE,
                                                                         m_convertedValue) == true) {
      accept();
    }
    else {
      MessageBox::warning(this,
                          tr("Invalid Variable Setup"),
                          tr("Cannot add variable \'%1\'. "
                             "Its name is invalid or is already used.").arg(m_ui->m_txtName->text()));
    }
  });

  return QDialog::exec();
}

int FormEditVariable::execEdit(const QString &name, const QString &value,
                               const QString &description, const QString &type,
                               const MemoryPlace::Type &var_type) {
  // Setup the form.
  m_originalName = name;
  m_ui->m_txtName->setText(name);
  m_ui->m_txtName->setEnabled(false);
  m_ui->m_txtValue->setText(value);
  m_ui->m_txtDescription->setText(description);
  m_ui->m_cmbType->setCurrentIndex(m_mappedTypes[type]);
  setWindowTitle(tr("Edit Variable"));

  // User cannot edit description of special variable, just value.
  m_ui->m_txtDescription->setEnabled(var_type != MemoryPlace::SPECIAL_VARIABLE);

  // Set initial focus.
  m_ui->m_txtValue->setFocus(Qt::TabFocusReason);

  connect(m_ui->m_btnBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, [=] () {
    if (CalculatorWrapper::getInstance().getCalculator()->editMemoryPlace(m_ui->m_txtName->text(),
                                                                          m_ui->m_txtDescription->text(),
                                                                          var_type,
                                                                          m_convertedValue) == true) {
      accept();
    }
    else {
      MessageBox::warning(this,
                          tr("Invalid Variable Setup"),
                          tr("Cannot edit variable \'%1\'. "
                             "Its value is not well-formed or another error occured.").arg(m_ui->m_txtName->text()));
    }
  });

  return QDialog::exec();
}
