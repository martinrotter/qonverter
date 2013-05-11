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

#include <QFile>
#include <QTextStream>

#include "date.h"
#include "formabout.h"
#include "mpDefines.h"


FormAbout::FormAbout(QWidget *parent) : QDialog(parent), m_ui(new Ui::FormAbout) {
  m_ui->setupUi(this);

  // Set flags.
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);

  // Load information from embedded text files.
  QTextStream str;
  QFile file;
  str.setDevice(&file);

  file.setFileName(APP_INFO_PATH + "/COPYING_GNU_GPL_HTML");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_ui->m_txtLicenseGnu->setText(str.readAll());
  }
  else {
    m_ui->m_txtLicenseGnu->setText(tr("License not found."));
  }
  file.close();

  file.setFileName(APP_INFO_PATH + "/COPYING_GNU_LGPL_HTML");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_ui->m_txtLicenseGnuLgpl->setText(str.readAll());
  }
  else {
    m_ui->m_txtLicenseGnuLgpl->setText(tr("License not found."));
  }
  file.close();

  file.setFileName(APP_INFO_PATH + "/COPYING_BSD");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_ui->m_txtLicenseBsd->setText(str.readAll());
  }
  else {
    m_ui->m_txtLicenseBsd->setText(tr("License not found."));
  }
  file.close();

  file.setFileName(APP_INFO_PATH + "/COPYING_FLAGS");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_ui->m_txtLicenseFlags->setText(str.readAll());
  }
  else {
    m_ui->m_txtLicenseFlags->setText(tr("License not found."));
  }
  file.close();

  file.setFileName(APP_INFO_PATH + "/AUTHORS");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_ui->m_txtThanks->setText(str.readAll());
  }
  else {
    m_ui->m_txtThanks->setText(tr("Thanks to not found."));
  }
  file.close();

  file.setFileName(APP_INFO_PATH + "/CHANGELOG");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_ui->m_txtChangelog->setText(str.readAll());
  }
  else {
    m_ui->m_txtChangelog->setText(tr("Changelog not found."));
  }
  file.close();

  // Set other informative texts.
  m_ui->m_lblDesc->setText(tr("<b>Qonverter</b><br>"
                              "<b>Version:</b> %1 (build on %2 with CMake %3)<br>"
                              "<b>Revision:</b> %4<br>"
                              "<b>Build date:</b> %5<br>"
                              "<b>Qt:</b> %6 (compiled against %7)<br>"
                              "<b>muParserX:</b> %8").arg(qApp->applicationVersion(),
                                                          CMAKE_SYSTEM,
                                                          CMAKE_VERSION,
                                                          APP_REVISION,
                                                          Date::fromString(QString("%1 %2").arg(__DATE__,
                                                                                                __TIME__)).toString(Qt::DefaultLocaleShortDate),
                                                          QT_VERSION_STR,
                                                          qVersion(),
                                                          QString::fromStdWString(MUP_PARSER_VERSION)));

  m_ui->m_txtInfo->setText(tr("<body>Qonverter is simple (yet powerful) calculator with some unusual functions."
                              "<br><br>This software is distributed under the terms of GNU General Public License, version 3 or later."
                              "<br><br>Contacts:"
                              "<ul><li><a href=\"mailto://rotter.martinos@gmail.com\">rotter.martinos@gmail</a>  ~email</li>"
                              "<li><a href=\"http://code.google.com/p/qonverter\">code.google.com/p/qonverter</a> ~website</li></ul>"
                              "You can obtain source code for Qonverter from its website."
                              "<br><br><br>Copyright 2011-%1 Martin Rotter</body>").arg(QDateTime::currentDateTime().date().year()));
}

FormAbout::~FormAbout() {
  delete m_ui;
}
