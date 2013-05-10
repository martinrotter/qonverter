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

#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QMainWindow>
#include <QPointer>

#include "ui_formmain.h"


namespace Ui {
  class FormMain;
}

class SystemTrayIcon;
class FormVariables;

class FormMain : public QMainWindow {
    Q_OBJECT

  public:
    // Constructors and desctructors.
    explicit FormMain(QWidget *parent = 0);
    ~FormMain();

  protected:
    // Creates menus and icons.
    void createTrayIcon();
    void createMenus();

#if defined(Q_OS_WIN)
    void moveEvent(QMoveEvent *event);
#endif

    // Creates various connections.
    void createGuiConnections();
    void createAppConnections();

    // Loads settings and sets main window accordingly.
    void loadSettings();

  protected slots:
    // Saves application state and does cleanup.
    void saveBeforeQuit();

  public slots:
    // Hides the window.
    void hideWindow();

    // Displays window with visible focus.
    void showWindow();

    // Display/hides window.
    void switchWindowVisibility();

  private:
    QMenu *m_trayMenu;
    Ui::FormMain *m_ui;
    QActionGroup *m_componentSwitcher;
    QPointer<SystemTrayIcon> m_trayIcon;
};

#endif // FORMMAIN_H
