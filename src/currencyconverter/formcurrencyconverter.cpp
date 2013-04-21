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

#include "messagebox.h"
#include "formcurrencyconverter.h"
#include "currencyconverter.h"


FormCurrencyConverter *FormCurrencyConverter::s_instance;

FormCurrencyConverter::FormCurrencyConverter(QWidget *parent)
  : QWidget(parent), m_ui(new Ui::FormCurrencyConverter) {
  m_ui->setupUi(this);

  // Needed because classical singleton fails due to UI files behavior.
  s_instance = this;

  // Make sure that currency converter converts input if needed
  // and vice versa,
  connect(this, &FormCurrencyConverter::conversionWanted,
          &CurrencyConverter::getInstance(), &CurrencyConverter::convert);

  connect(&CurrencyConverter::getInstance(), &CurrencyConverter::converted,
          this, &FormCurrencyConverter::manageConvertedResult);

  // Make sure that conversion is requested if one of the currencies changes.
  connect(m_ui->m_listInput, &QListWidget::currentRowChanged, [=] () {
    m_ui->m_spinInput->setSuffix(QString(" ") + (m_ui->m_listInput->selectedItems().isEmpty() ?
                                                   "" :
                                                   m_ui->m_listInput->currentItem()->text()));
    requestConversion();
  });

  connect(m_ui->m_listOutput, &QListWidget::currentRowChanged, [=] () {
    m_ui->m_spinOutput->setSuffix(QString(" ") + (m_ui->m_listOutput->selectedItems().isEmpty() ?
                                                    "" :
                                                    m_ui->m_listOutput->currentItem()->text()));
    requestConversion();
  });

  connect(m_ui->m_spinInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          this, &FormCurrencyConverter::requestConversion);

  // Rates update message.
  connect(&CurrencyConverter::getInstance(), &CurrencyConverter::ratesUpdated,
          [=] (CurrencyConverter::UpdateStatus status) {

    switch (status) {
      case CurrencyConverter::NOT_DOWNLOADED:
        MessageBox::warning(this, tr("Rates Not Updated"),
                             tr("Currency rates were not downloaded successfully. Check your internet connection."));
        break;
      case CurrencyConverter::NOT_SAVED:
        MessageBox::warning(this, tr("Rates Not Updated"),
                             tr("Currency rates were not saved successfully. Target location is not writable."));
        break;
      case CurrencyConverter::OK:
        initialize();
        MessageBox::information(this, tr("Rates Updated"),
                                tr("Currency rates were updated and loaded successfully."));
        break;
      default:
        break;

    }
  });

  connect(m_ui->m_btnUpdateCurrencies, static_cast<void (QAbstractButton::*)(bool)>(&QAbstractButton::clicked),
          &CurrencyConverter::getInstance(), &CurrencyConverter::updateRates);

  // Make initial initialization.
  initialize();
}

void FormCurrencyConverter::requestConversion() {
  if (m_ui->m_listInput->selectedItems().isEmpty() == false &&
      m_ui->m_listOutput->selectedItems().isEmpty() == false) {
    emit conversionWanted(m_ui->m_spinInput->value(),
                          m_ui->m_listInput->currentItem()->text(),
                          m_ui->m_listOutput->currentItem()->text());
  }
}

void FormCurrencyConverter::manageConvertedResult(double result) {
  m_ui->m_spinOutput->setValue(result);
}

FormCurrencyConverter::~FormCurrencyConverter() {
  delete m_ui;
}

void FormCurrencyConverter::initialize() {
  // Load currency file and initialize converter.
  CurrencyConverter::getInstance().initialize();

  // Initialize form.
  m_ui->m_lblBank->setText(CurrencyConverter::getInstance().getBank());
  m_ui->m_lblDate->setText(CurrencyConverter::getInstance().getDate().toString(Qt::DefaultLocaleShortDate));

  QStringList currency_names = CurrencyConverter::getInstance().getCurrencyNames();
  m_ui->m_listInput->clear();
  m_ui->m_listInput->addItems(currency_names);
  m_ui->m_listInput->sortItems();
  m_ui->m_listOutput->clear();
  m_ui->m_listOutput->addItems(currency_names);
  m_ui->m_listOutput->sortItems();

  m_ui->m_listInput->setCurrentRow(0);
  m_ui->m_listOutput->setCurrentRow(0);
  m_ui->m_spinInput->setValue(0.0);
}

QDoubleSpinBox *FormCurrencyConverter::getInput() {
  return m_ui->m_spinInput;
}

FormCurrencyConverter *FormCurrencyConverter::getInstance() {
  return s_instance;
}
