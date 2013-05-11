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

#include "unitconverter.h"


QScopedPointer<UnitConverter> UnitConverter::s_instance;

UnitConverter::UnitConverter(QObject *parent) : QObject(parent) {
  loadUnitsAndMangitudes();
}

UnitConverter::~UnitConverter() {
  qDebug("Deleting unit converter.");
}

void UnitConverter::loadUnitsAndMangitudes() {
  // Acceleration.
  QList<double> list_accel; list_accel << 1 << 0.3048 << 0.01 << 9.80665;
  // Angle.
  QList<double> list_angle; list_angle << 1 << 0.0157 << 0.01745;
  // Area.
  QList<double> list_area; list_area << 1 << 1.0e-06 << 1.0e-04 << 1.0e-02 << 100 <<
                                        1.0e+04 << 1.0e+06 << 0.0929 << 0.83612 << 2.58999e+06 <<
                                        1011.7141056 << 440 << 3930;
  // Density.
  QList<double> list_density; list_density << 1 << 1000 << 0.001 << 1.0e-06 << 119.826 <<
                                              99.7764 << 0.59327 << 16.0185 << 2.76799e+04;
  // Electric current.
  QList<double> list_current; list_current << 1 << 10.0 << 3.335641e-10;
  // Electric voltage.
  QList<double> list_voltage; list_voltage << 1 << 1.0e-08 << 1000 << 1.00034 << 299.793;
  // Energy & work.
  QList<double> list_work; list_work << 1 << 1054.54 << 4.1868 << 1.899100534716e+03 <<
                                        1.60219e-19 << 4.359744e-18 << 3.6e+06;
  // Force.
  QList<double> list_force; list_force << 1 << 0.0098 << 9.80665;
  // Information entropy.
  QList<double> list_entropy; list_entropy << 1 << 1/8.0 << 1.0e+03 << 1.0e+06 << 1.0e+09 <<
                                              1.0e+12 << 1024.0 << 1024.0 * 1024 <<
                                              1024.0 * 1024 * 1024 << 1024.0 * 1024 * 1024 * 1024 <<
                                              7.655952e+23 << 0.5;
  // Length.
  QList<double> list_length; list_length << 1 << 0.001 << 0.01 << 1000 << 1609.34 <<
                                            1.49598e+11 << 1.0e-15 << 0.3048 << 0.0254 <<
                                            1852.0 << 0.9144;
  // Luminance.
  QList<double> list_luminance; list_luminance << 1 << 3183.1 << 1.0e+04;
  // Luminosity.
  QList<double> list_luminosity; list_luminosity << 1 << 1.019 << 1.084 << 1.0114 <<
                                                    20.23 << 1.0023 << 1.16 << 9.704 <<
                                                    9.91 << 0.92;
  // Mass.
  QList<double> list_mass; list_mass << 1 << 1/1000.0 << 1.0e-06 << 100.0 << 1000.0 <<
                                        0.37324 << 0.45359 << 0.028349523125 <<
                                        0.000205196548333 << 0.0002 << 1.66057e-27 <<
                                        0.0012959782 << 11.49875 << 14.70588235 <<
                                        0.0017718451953125 << 0.00006479891 << 45.359237 <<
                                        907.18474 << 50;
  // Power.
  QList<double> list_power; list_power << 1 << 745.7 << 0.29307;
  // Pressure.
  QList<double> list_pressure; list_pressure << 1 << 101325.0 << 9.80665e+04 << 1.0e+05 << 133.3224 << 6.894757e+03;
  // Time.
  QList<double> list_time; list_time << 1 << 1.0e-06 << 1.0e-03 << 60.0 << 3600.0 << 8.64e+04 << 6.048e+05 << 2.62975e+06 << 3.15581e+07 << 3.15569e+08 << 3.15569e+09 << 31.5569e9 << 10/3.0 << 1.0e-13 << 31556925.71 << 31558149.54;
  // Velocity.
  QList<double> list_velocity; list_velocity << 1 << 0.01 << 2.77778e-04 << 1/3.6 << 0.44704 << 0.51444 << 0.3048 << 340.278 << 299792458.0 << 340.278;
  // Volume.
  QList<double> list_volume; list_volume << 1 << 1.0e-06 << 1.0e-03 << 0.1 << 473.176473e-06 << 568.261e-06 << 0.00378 << 0.00454 << 0.15898;
  // Torque (moment of force)
  QList<double> list_moment; list_moment << 1 << 0.101971621 << 0.1129848290276167 << 0.0421401100938048 << 1.3558179483314004;
  // Electric charge
  QList<double> list_charge; list_charge << 1 << 96485.3383 << 3.335641e-10 << 10;
  // Illuminance
  QList<double> list_illuminance; list_illuminance << 1 << 10000 << 1550.0031 << 10.763910417;
  // Frequency
  QList<double> list_frequency; list_frequency << 1 << 1/60.0 << 0.15915494327;

  // Put everything into single list.
  m_unitValues << list_accel << list_angle << list_area << list_density <<
                  list_current << list_voltage << list_work << list_force <<
                  list_entropy << list_length << list_luminance << list_luminosity <<
                  list_mass << list_power << list_pressure << list_time << list_velocity <<
                  list_volume << list_moment << list_charge << list_illuminance <<
                  list_frequency;

  QList<QString> acceleration; acceleration << QObject::tr("m/s²") << QObject::tr("ft/s²") << QObject::tr("gal") << QObject::tr("g (standard gravity)");
  QList<QString> angle; angle << QObject::tr("rad") << QObject::tr("grad") << QObject::tr("deg");
  QList<QString> area; area << QObject::tr("m²") << QObject::tr("mm²") << QObject::tr("cm²") << QObject::tr("dm²") << QObject::tr("a (are") << QObject::tr("ha (hectare)") <<
                               QObject::tr("km²") << QObject::tr("ft² (square foot)") << QObject::tr("yd² (square yard)") <<
                               QObject::tr("mi² (square mile)") << QObject::tr("rood (rood)") << QObject::tr("cuerda [Guatemala]") <<
                               QObject::tr("cuerda [Puerto Rico]");
  QList<QString> density; density << QObject::tr("kg/m3") << QObject::tr("g/cm3") << QObject::tr("g/m3") << QObject::tr("mg/m3") <<
                                     QObject::tr("lb/US gal (pound per gallon)") <<
                                     QObject::tr("lb/UK gal (pound per gallon)") <<
                                     QObject::tr("lb/cu.yd (pound per cubic yard)") <<
                                     QObject::tr("lb/cu.ft (pound per cubic foot)") <<
                                     QObject::tr("lb/cu.in (pound per cubic inch)");
  QList<QString> current; current << QObject::tr("A (Amper)") << QObject::tr("abamp (electromag. unit)") << QObject::tr("esu/s");
  QList<QString> voltage; voltage << QObject::tr("V (Volt)") << QObject::tr("aV (abVolt)") << QObject::tr("kV (kiloVolt)") <<
                                     QObject::tr("V int. (international Volt)") << QObject::tr("sV (statVolt)");
  QList<QString> work; work << QObject::tr("j (Joule)") << QObject::tr("btu-iso (Brittish thermal unit)") <<
                               QObject::tr("cal (calorie)") << QObject::tr("chu-it (Celsius heat unit)") <<
                               QObject::tr("eV (electronVolt)") << QObject::tr("eh (atomic unit of energy)") <<
                               QObject::tr("kWh (kiloWatt-hour)");
  QList<QString> force; force << QObject::tr("N (Newton)") << QObject::tr("pond") << QObject::tr("kilopond");
  QList<QString> entropy; entropy << QObject::tr("B (byte)") << QObject::tr("b (bit)") << QObject::tr("kB (kilobyte)") <<
                                     QObject::tr("MB (megabyte)") << QObject::tr("GB (gigabyte)") << QObject::tr("TB (terabyte)") <<
                                     QObject::tr("KiB (kibibyte)") << QObject::tr("MiB (mebibyte)") << QObject::tr("GiB (gibibyte)") <<
                                     QObject::tr("TiB (tebibyte)") << QObject::tr("J/K") << QObject::tr("Nibble");
  QList<QString> length; length << QObject::tr("m") << QObject::tr("mm") << QObject::tr("cm") << QObject::tr("km") << QObject::tr("mi (mile)") <<
                                   QObject::tr("AU (astronom. unit)") << QObject::tr("fm (fermi)") << QObject::tr("ft (foot)") <<
                                   QObject::tr("in (inch)") << QObject::tr("nmi (intern. nautical mile)") << QObject::tr("yd (yard)");
  QList<QString> luminance; luminance << QObject::tr("cd/m²") << QObject::tr("L (Lambert)") << QObject::tr("sb (stilb)");
  QList<QString> luminosity; luminosity << QObject::tr("cd (candela)") <<
                                           QObject::tr("cp (international candle)") <<
                                           QObject::tr("paraffin candle") << QObject::tr("DB (decimal candle)") <<
                                           QObject::tr("violle (Violle's candle)") <<
                                           QObject::tr("pentane candle") << QObject::tr("spermacet candle") <<
                                           QObject::tr("carcel lamp") << QObject::tr("Harcourt's lamp") <<
                                           QObject::tr("HK (Hefner's candle)");
  QList<QString> mass; mass << QObject::tr("kg (kilogram)") << QObject::tr("g (gram)") << QObject::tr("mg (miligram)") <<
                               QObject::tr("q (quintal)") << QObject::tr("t (tonne)") << QObject::tr("pound (troy)") <<
                               QObject::tr("pound (avoirdupois)") << QObject::tr("ounce (avoirdupois)") <<
                               QObject::tr("kt (carat)") << QObject::tr("ct (metric carat)") << QObject::tr("u (atomic mass unit)") <<
                               QObject::tr("sc. (apoth. scruple)") << QObject::tr("@ (arroba [ESP])") << QObject::tr("arroba [POR] ") <<
                               QObject::tr("dr (dram)") << QObject::tr("gr (grain)") << QObject::tr("cwt (short hundredweight US)") <<
                               QObject::tr("ton (short ton US)") << QObject::tr("ztr (zentner)");
  QList<QString> power; power << QObject::tr("w (watt)") << QObject::tr("hp (horsepower)") << QObject::tr("BTU/h");
  QList<QString> pressure; pressure << QObject::tr("pa (pascal)") << QObject::tr("atm (standard atmosphere)") <<
                                       QObject::tr("at (technical atmosphere)") << QObject::tr("bar") << QObject::tr("torr-mmHg") <<
                                       QObject::tr("psi (pound per square inch)");
  QList<QString> time; time << QObject::tr("s") << QObject::tr("µs") << QObject::tr("ms") << QObject::tr("min") << QObject::tr("h") << QObject::tr("day") << QObject::tr("week") <<
                               QObject::tr("month") << QObject::tr("year") << QObject::tr("decade") << QObject::tr("century") << QObject::tr("millenium") <<
                               QObject::tr("hl (helek [Hebrew])") << QObject::tr("S (Svedberg)") << QObject::tr("a (tropical year)") <<
                               QObject::tr("sidereal time");
  QList<QString> velocity; velocity << QObject::tr("m/s") << QObject::tr("cm/h") << QObject::tr("m/h") << QObject::tr("km/h") << QObject::tr("mp/h") <<
                                       QObject::tr("kn (knot)") << QObject::tr("ft/s") << QObject::tr("M (mach at sea level)") <<
                                       QObject::tr("c (speed of light in vacuum)") << QObject::tr("s (speed of light in air)");
  QList<QString> volume; volume << QObject::tr("m³") << QObject::tr("ml/cm³") << QObject::tr("l/dm³") << QObject::tr("hl") << QObject::tr("pint (USA)") <<
                                   QObject::tr("pint (UK)") << QObject::tr("gallon (USA)") << QObject::tr("gallon (UK)") <<
                                   QObject::tr("bll (barrel of petroleum)");
  QList<QString> moment; moment << QObject::tr("N·m (Newton metre)") << QObject::tr("m kg (metre kilogram)") <<
                                   QObject::tr("in lbf (inch-pound force)") <<
                                   QObject::tr("ft pdl (foot-poundal)") << QObject::tr("ft lbf (foot-pound force)");
  QList<QString> charge; charge << QObject::tr("C (Coulomb)") << QObject::tr("F (Faraday)") << QObject::tr("statC (statcoulomb)") <<
                                   QObject::tr("abC (abCoulomb)");
  QList<QString> illuminance; illuminance << QObject::tr("lx (lux)") << QObject::tr("ph (phot)") <<
                                             QObject::tr("lm/in2 (lumen per square inch)") << QObject::tr("fc (footcandle)");
  QList<QString> frequency; frequency << QObject::tr("Hz (Hertz)") << QObject::tr("rpm (revolutions per minute)") <<
                                         QObject::tr("rad/s");

  // Put all unit names into single list.
  m_unitNames << acceleration << angle << area << density <<
                 current << voltage << work << force <<
                 entropy << length << luminance << luminosity <<
                 mass << power << pressure << time << velocity <<
                 volume << moment << charge << illuminance <<
                 frequency;

  // Put all magnitude names into single list.
  m_magnitudeNames << QObject::tr("Acceleration") << QObject::tr("Angle") << QObject::tr("Area") <<
                      QObject::tr("Density") << QObject::tr("El. current") <<
                      QObject::tr("El. voltage") << QObject::tr("Energy & work") <<
                      QObject::tr("Force") << QObject::tr("Digital information") <<
                      QObject::tr("Length") << QObject::tr("Luminance") <<
                      QObject::tr("Luminosity") << QObject::tr("Mass") <<
                      /*QObject::tr("Numeric systems") <<*/ QObject::tr("Power") <<
                      QObject::tr("Pressure") /*<< QObject::tr("Temperature")*/ <<
                      QObject::tr("Time") << QObject::tr("Velocity") <<
                      QObject::tr("Volume") << QObject::tr("Torque") <<
                      QObject::tr("El. charge") << QObject::tr("Illuminance") <<
                      QObject::tr("Frequency");
}

QList<QString> UnitConverter::getMagnitudes() {
  return m_magnitudeNames;
}

QList<QString> UnitConverter::getUnits(int i) {
  return m_unitNames.at(i);
}

UnitConverter &UnitConverter::getInstance() {
  if (s_instance.isNull()) {
    s_instance.reset(new UnitConverter());
  }

  return *s_instance;
}

void UnitConverter::convert(int magnitude_index, int input_unit,
                            int output_unit, const QString &input_value) {
  emit converted(convertByMultiplication(magnitude_index,
                                         input_unit,
                                         output_unit,
                                         input_value));
}

QString UnitConverter::convertByMultiplication(int magnitude_index,
                                               int input_unit,
                                               int output_unit,
                                               const QString &input_value) {
  double input_number = input_value.toDouble();
  return QString::number(input_number *
                         m_unitValues[magnitude_index][input_unit] /
                         m_unitValues[magnitude_index][output_unit],
                         'g',
                         10);
}
