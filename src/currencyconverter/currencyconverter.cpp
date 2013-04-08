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

#include <QDir>
#include <QDomDocument>
#include <QNetworkReply>

#include "currencyconverter.h"
#include "defs.h"
#include "downloader.h"


QScopedPointer<CurrencyConverter> CurrencyConverter::s_instance;

CurrencyConverter::CurrencyConverter(QObject *parent) : QObject(parent) {
}

CurrencyConverter::~CurrencyConverter() {
  qDebug("Deleting currency converter");
}

CurrencyConverter &CurrencyConverter::getInstance() {
  if (s_instance.isNull() == true) {
    s_instance.reset(new CurrencyConverter());
  }

  return *s_instance;
}

QList<QString> CurrencyConverter::getCurrencyNames() {
  return m_currencies.keys();
}

QString CurrencyConverter::getBank() {
  return m_bankName;
}

QDateTime CurrencyConverter::getDate() {
  return m_date;
}

void CurrencyConverter::initialize() {
  QString custom_rates = QDir::homePath() + QDir::separator() + APP_LOW_H_NAME +
                         QDir::separator() + APP_CUSTOM_RATES_PATH +
                         QDir::separator() + APP_CUSTOM_RATES_FILE;

  QFile custom_rates_file(custom_rates);
  QByteArray rates_data;

  if (custom_rates_file.exists() == true &&
      custom_rates_file.open(QIODevice::Text | QIODevice::ReadOnly) == true) {
    rates_data = custom_rates_file.readAll();
    custom_rates_file.flush();
    custom_rates_file.close();
  }
  else {
    QFile default_rates_file(APP_RATES_PATH);
    default_rates_file.open(QIODevice::Text | QIODevice::ReadOnly);
    rates_data = default_rates_file.readAll();
    default_rates_file.flush();
    default_rates_file.close();
  }

  QDomDocument xml_file;
  xml_file.setContent(rates_data);

  QDomElement bank = xml_file.documentElement()
                     .firstChildElement("gesmes:Sender")
                     .firstChildElement("gesmes:name");
  QDomElement root = xml_file.documentElement()
                     .firstChildElement("Cube")
                     .firstChildElement("Cube");

  m_bankName = bank.text();
  m_date = QDateTime::fromString(root.attribute("time"), "yyyy-MM-dd");

  // Load rates data.
  m_currencies.clear();
  m_currencies.insert("EUR", 1.0);

  for (QDomElement n = root.firstChildElement(); !n.isNull(); n = n.nextSiblingElement()) {
    m_currencies.insert(n.attribute("currency"),
                        n.attribute("rate").toDouble());
  }
}

void CurrencyConverter::convert(double input_cash, const QString &input_currency, const QString &output_currency) {
  emit converted(input_cash / m_currencies[input_currency] * m_currencies[output_currency]);
}

void CurrencyConverter::updateRates() {
  QByteArray rates_file_data;
  QNetworkReply::NetworkError download_result = Downloader::downloadFile(APP_RATES_URL,
                                                                         rates_file_data,
                                                                         1000);

  if (download_result == QNetworkReply::NoError) {
    QDir().mkdir(QDir::homePath() + QDir::separator() + APP_LOW_H_NAME +
                 QDir::separator() + APP_CUSTOM_RATES_PATH);

    QFile output_file(QDir::homePath() + QDir::separator() + APP_LOW_H_NAME +
                      QDir::separator() + APP_CUSTOM_RATES_PATH +
                      QDir::separator() + APP_CUSTOM_RATES_FILE);

    if (output_file.open(QIODevice::Text | QIODevice::WriteOnly) == true) {
      output_file.write(rates_file_data);
      output_file.flush();
      output_file.close();
      emit ratesUpdated(OK);
    }
    else {
      emit ratesUpdated(NOT_SAVED);
    }
  }
  else {
    emit ratesUpdated(NOT_DOWNLOADED);
  }
}
