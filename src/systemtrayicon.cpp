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

#include "systemtrayicon.h"
#include "formmain.h"
#include "settings.h"
#include "defs.h"
#include "messagebox.h"


SystemTrayIcon::SystemTrayIcon(const QIcon &icon, QObject *parent)
  : QSystemTrayIcon(icon, parent) {
}

void SystemTrayIcon::showMessage(const QString &title,
                                 const QString &msg,
                                 MessageIcon icon, int msecs) {
  if (Settings::value(APP_CFG_GUI, "tray_icon_enabled", false).toBool() &&
      QSystemTrayIcon::isSystemTrayAvailable()) {
    QSystemTrayIcon::showMessage(title, msg, icon, msecs);
  }
  // Tray icon is disable or is not available.
  // Display simple message box in that case.
  else {
    switch (icon) {
      case NoIcon:
      case Information:
        MessageBox::information(FormMain::getInstance(),
                                title,
                                msg);
        break;
      case Warning:
        MessageBox::warning(FormMain::getInstance(),
                            title,
                            msg);
        break;
      case Critical:
        MessageBox::warning(FormMain::getInstance(),
                            title,
                            msg);
        break;
      default:
        break;
    }
  }
}
