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

#ifndef GSETTINGS_H
#define GSETTINGS_H

#include <QSettings>


class Settings {
  private:
    // We use QPointer instead of QScopedPointer
    // because of late s_instance usage in QApplication::aboutToQuit() listeners.
    static QPointer<QSettings> s_instance;

  public:
    // Getter/setter for settings values.
    static QVariant value(const QString &section,
                          const QString &key,
                          const QVariant &default_value = QVariant());

    static void setValue(const QString &section,
                         const QString &key,
                         const QVariant &value);

    // It's better to cleanup settings manually via this function.
    static void deleteSettings();

    // Synchronises settings.
    static QSettings::Status checkSettings();

  protected:
    // Creates settings file in correct location.
    static QSettings::Status setupSettings();
};

#endif // GSETTINGS_H
