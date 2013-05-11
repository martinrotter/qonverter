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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>


// Static methods in this class wrap QMessageBox.
// Reason is that that static methods of QMessageBox
// construct application-modal dialogs but i want
// window-modal dialogs.
class MessageBox {
  private:
    MessageBox();

  public:
    // Enhanced, informative text is now part of the method
    // signature, thus, there is no reason to contruct
    // QMessageBox instances manually via constructor.
    static QMessageBox::StandardButton question(QWidget * parent,
                                                const QString &title,
                                                const QString &text,
                                                const QString &informative_text,
                                                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                QMessageBox::StandardButton default_button = QMessageBox::NoButton);

    static QMessageBox::StandardButton custom(QWidget * parent,
                                              const QString &title,
                                              const QString &text,
                                              QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                              QMessageBox::StandardButton default_button = QMessageBox::NoButton,
                                              const QPixmap &pixmap = QPixmap());


    static QMessageBox::StandardButton warning(QWidget * parent,
                                               const QString &title,
                                               const QString &text,
                                               QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                               QMessageBox::StandardButton default_button = QMessageBox::NoButton);

    static QMessageBox::StandardButton information(QWidget * parent,
                                                   const QString &title,
                                                   const QString &text,
                                                   QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                   QMessageBox::StandardButton default_button = QMessageBox::NoButton);
};

#endif // MESSAGEBOX_H
