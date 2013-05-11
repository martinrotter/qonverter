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
    Copyright 2010 Nokia Corporation
*/

#include "fontdialog.h"


FontDialog::FontDialog() {
}

QFont FontDialog::getFont(bool *ok, const QFont &initial,
                          QWidget *parent, const QString &title,
                          QFontDialog::FontDialogOptions options) {
  QFontDialog dlg(parent);
  dlg.setOptions(options);
  dlg.setCurrentFont(initial);
  dlg.setWindowModality(Qt::WindowModal);
  if (!title.isEmpty()) {
    dlg.setWindowTitle(title);
  }

  int ret = dlg.exec() || (options & QFontDialog::NoButtons);
  if (ok) {
    *ok = !!ret;
  }
  if (ret) {
    return dlg.selectedFont();
  } else {
    return initial;
  }
}
