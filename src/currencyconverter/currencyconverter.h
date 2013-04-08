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

#ifndef CURRENCYCONVERTER_H
#define CURRENCYCONVERTER_H

#include <QObject>
#include <QDateTime>


class CurrencyConverter : public QObject {
    Q_OBJECT

  public:
    enum UpdateStatus {
      NOT_DOWNLOADED,
      NOT_SAVED,
      OK
    };

    // Contructors and destructors.
    explicit CurrencyConverter(QObject *parent = 0);
    ~CurrencyConverter();

    // Checks for existence of currency files,
    // choses it and loads currency names and values.
    void initialize();

    // Obvious.
    QList<QString> getCurrencyNames();
    QString getBank();
    QDateTime getDate();

    static CurrencyConverter &getInstance();

  public slots:
    // Converts input currency into output currency and reports the result via signal.
    void convert(double input_cash,
                 const QString &input_currency,
                 const QString &output_currency);

    // Updates rates and reports via signal.
    void updateRates();

  signals:
    void ratesUpdated(UpdateStatus status);
    void converted(double result);

  private:
    QHash<QString, double> m_currencies;
    QString m_bankName;
    QDateTime m_date;

    static QScopedPointer<CurrencyConverter> s_instance;
};

#endif // CURRENCYCONVERTER_H
