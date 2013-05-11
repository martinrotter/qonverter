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

#include "doublespinbox.h"
#include "lineedit.h"


DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent) {
  // Make sure that English locale is used
  // because dot is better for separator than comma is.
  setLocale(QLocale::English);

  // Replace default line edit component with custom one.
  LineEdit *line_edit = new LineEdit();
  line_edit->setMinimumHeight(line_edit->minimumSizeHint().height());
  line_edit->setStyleSheet("LineEdit { background: transparent; }");

  // Clear button is not needed.
  line_edit->setClearButtonEnabled(false);

  // Set line edit as editing component for this spin box.
  setLineEdit(line_edit);
}
