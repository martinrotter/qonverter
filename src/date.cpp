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

#include <QLocale>
#include <QRegExp>
#include <QStringList>

#include "date.h"


QDateTime Date::fromString(const QString &date_time) {
  QString date = date_time.simplified();
  QDateTime dt;
  QString temp;
  QLocale locale(QLocale::C);
  QStringList date_patterns;
  date_patterns << "yyyy-MM-ddTHH:mm:ss" << "MMM dd yyyy hh:mm:ss" <<
                   "MMM hd yyyy hh:mm:ss" << "ddd, dd MMM yyyy HH:mm:ss" <<
                   "dd MMM yyyy" << "yyyy-MM-dd HH:mm:ss.z" << "yyyy-MM-dd";

  // Iterate over patterns and check if input date/time matches the pattern.
  foreach (QString pattern, date_patterns) {
    temp = date.left(pattern.size());
    dt = locale.toDateTime(temp, pattern);
    if (dt.isValid()) {
      return dt;
    }
  }

  qWarning("Problem with parsing date '%s', returning invalid QDateTime instance.",
           qPrintable(date));
  return QDateTime();
}
