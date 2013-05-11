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

#include <QString>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QWidget>

#include "uifactory.h"
#include "defs.h"


bool UiFactory::loadSkin(const QString &skin, QWidget *widget) {
  QStringList skin_parts = skin.split('/', QString::SkipEmptyParts);

  // Skin does not contain leading folder name or the actual skin file name.
  if (skin_parts.size() != 2) {
    qDebug("Loading of sking %s failed because skin name does not contain "
           "base folder name or the actual skin name.",
           qPrintable(skin));
    return false;
  }

  // Create needed variables and create QFile object representing skin contents.
  QString skin_folder = skin_parts.at(0);
  QString skin_file_name = skin_parts.at(1);
  QFile skin_file_full_path(APP_SKIN_PATH + QDir::separator() +
                            skin_folder + QDir::separator() + skin_file_name);

  // Try to load up skin contents.
  if (!skin_file_full_path.open(QIODevice::Text | QIODevice::ReadOnly)) {
    qDebug("Loading of sking %s failed because "
           "skin file does not exist or is not readable.",
           qPrintable(skin_file_name));
    return false;
  }

  QTextStream str(&skin_file_full_path);
  QString skin_data;

  // Read skin data from file and close it.
  skin_data.append(str.readAll());
  skin_file_full_path.close();
  skin_file_full_path.deleteLater();

  // Here we use "/" instead of QDir::separator() because CSS2.1 url field
  // accepts '/' as path elements separator.
  //
  // "##" is placeholder for the actual path to skin file. This is needed for using
  // images within the QSS file.
  skin_data = skin_data.replace("##",
                                APP_SKIN_PATH + "/" + skin_folder + "/images");

  // If widget was passed, then set its stylesheet only.
  if (widget != nullptr) {
    widget->setStyleSheet(skin_data);
  }
  // Set stylesheet for whole application.
  else {
    qApp->setStyleSheet(skin_data);
  }

  // Everything succeeded.
  qDebug("Skin %s was loaded successfully.", qPrintable(skin_file_name));
  return true;
}
