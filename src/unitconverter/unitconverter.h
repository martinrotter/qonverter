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

#ifndef UNITCONVERTER_H
#define UNITCONVERTER_H

#include <QObject>


class UnitConverter : public QObject {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit UnitConverter(QObject *parent = 0);
    ~UnitConverter();

    // Returns list of magnitude names.
    QList<QString> getMagnitudes();

    // Returns list of units contained in magnitude with index i.
    QList<QString> getUnits(int i);

    static UnitConverter &getInstance();

  protected:
    // Loads magnitudes and rates, including coefficients.
    // TODO: Perhaps do some loading from the database.
    void loadUnitsAndMangitudes();

    // Converter input_value according to selected mangitude and units
    // into output value, which is represented by QString.
    // Conversion is done by coefficient multiplication method.
    QString convertByMultiplication(int magnitude_index, int input_unit,
                                    int output_unit, const QString &input_value);

  public slots:
    // Converter input_value via correct conversion method.
    void convert(int magnitude_index, int input_unit,
                 int output_unit, const QString &input_value);

  signals:
    // Emitted if some value gets converted.
    void converted(const QString &result);

  private:
    // Holds coefficients of units.
    QList<QList<double> > m_unitValues;

    // Holds names of units.
    QList<QList<QString> > m_unitNames;

    // Holds names of magnitudes.
    QList<QString> m_magnitudeNames;

    static QScopedPointer<UnitConverter> s_instance;
};

#endif // UNITCONVERTER_H
