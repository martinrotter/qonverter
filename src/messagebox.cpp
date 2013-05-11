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


MessageBox::MessageBox() {
}

QMessageBox::StandardButton MessageBox::question(QWidget *parent,
                                                 const QString &title,
                                                 const QString &text,
                                                 const QString &informative_text,
                                                 QMessageBox::StandardButtons buttons,
                                                 QMessageBox::StandardButton default_button) {
  QMessageBox msg_box(QMessageBox::Question, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);
  msg_box.setInformativeText(informative_text);
  msg_box.setDefaultButton(default_button);

  return (QMessageBox::StandardButton) msg_box.exec();
}

QMessageBox::StandardButton MessageBox::custom(QWidget *parent,
                                               const QString &title,
                                               const QString &text,
                                               QMessageBox::StandardButtons buttons,
                                               QMessageBox::StandardButton default_button,
                                               const QPixmap &pixmap) {
  QMessageBox msg_box(QMessageBox::Warning, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);
  msg_box.setIconPixmap(pixmap);
  msg_box.setDefaultButton(default_button);

  return (QMessageBox::StandardButton) msg_box.exec();
}

QMessageBox::StandardButton MessageBox::warning(QWidget *parent,
                                                const QString &title,
                                                const QString &text,
                                                QMessageBox::StandardButtons buttons,
                                                QMessageBox::StandardButton default_button) {
  QMessageBox msg_box(QMessageBox::Warning, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);
  msg_box.setDefaultButton(default_button);

  return (QMessageBox::StandardButton) msg_box.exec();
}


QMessageBox::StandardButton MessageBox::information(QWidget *parent,
                                                    const QString &title,
                                                    const QString &text,
                                                    QMessageBox::StandardButtons buttons,
                                                    QMessageBox::StandardButton default_button) {
  QMessageBox msg_box(QMessageBox::Information, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);
  msg_box.setDefaultButton(default_button);

  return (QMessageBox::StandardButton) msg_box.exec();
}
