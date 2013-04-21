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

#include <QDesktopWidget>
#include <QApplication>
#include <QWindow>

#include "formmain.h"
#include "formabout.h"
#include "formsettings.h"
#include "formvariables.h"

#include "messagebox.h"
#include "settings.h"
#include "defs.h"
#include "calculatorwrapper.h"
#include "unitconverter.h"
#include "uifactory.h"
#include "extensions.h"
#include "stackedwidget.h"
#include "systemtrayicon.h"


FormMain::FormMain(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::FormMain) {
  m_ui->setupUi(this);

  // Create connections and initialize user interface.
  createMenus();
  createTrayIcon();

  createGuiConnections();
  createAppConnections();

  loadSettings();
}

FormMain::~FormMain() {
  // Delete all gui components.
  delete m_ui;
  delete m_componentSwitcher;
  delete m_trayMenu;

  // Make sure tray icon is deleted too.
  if (m_trayIcon.isNull() == false) {
    delete m_trayIcon.data();
  }
}

void FormMain::createTrayIcon() {
  // User has enabled tray icon.
  if (Settings::value(APP_CFG_GUI, "tray_icon_enabled", false).toBool() == true) {
    // Tray icon is really available.
    if (QSystemTrayIcon::isSystemTrayAvailable() == true) {
      // Make sure tray icon allocated only if it's not already loaded.
      if (m_trayIcon.isNull() == true) {
        // Tray icon is not created but should be used. Create it.
        m_trayIcon = new SystemTrayIcon(QIcon(":/graphics/qonverter.png"),
                                        qApp);
        //(*m_trayIcon).setContextMenu(m_trayMenu);
        (*m_trayIcon).setToolTip(QString("Qonverter ") + APP_VERSION);
        (*m_trayIcon).show();

        // We create connections here and not in createGuiConnections because of
        // switchable system tray.
        connect(m_trayIcon.data(), &SystemTrayIcon::activated,
                [=] (QSystemTrayIcon::ActivationReason reason) {
          switch (reason) {
            case QSystemTrayIcon::Trigger:
              // User clicked tray icon.
              switchWindowVisibility();
              break;
            case QSystemTrayIcon::Context: {
              // User requests tray icon context menu.
              // Check if context menu should be displayed.
              // It is not available if any modal dialog is opened.
              QWidget *active_modal_dialog = QApplication::activeModalWidget();

              if (active_modal_dialog == nullptr) {
                // No modal dialogs are visible, display the menu.
                m_trayMenu->exec(QCursor::pos());
              }
              else {
                // At least one modal dialog is shown.
                // Disable menu in this case.
                (*m_trayIcon).showMessage(QString("Qonverter ") + APP_VERSION,
                                          tr("Close opened modal dialogs first."),
                                          QSystemTrayIcon::Warning);
              }
              break;
            }
            default:
              break;
          }
        });
      }
    }
    else {
      MessageBox::warning(this, tr("System Tray Icon Not Available"),
                          tr("Your system doesn't seem to support system tray icon. Tray icon wasn't loaded and Qonverter runs in single window mode."));
    }
  }
  else {
    // Make sure tray icon is not loaded if user doesn't need it.
    if (m_trayIcon.isNull() == false) {
      delete m_trayIcon.data();
    };
  }
}

void FormMain::loadSettings() {
  // Display keypad and check correct mode in menu.
  m_ui->m_actionDisplayKeypad->setChecked(Settings::value(APP_CFG_CALC,
                                                          "keypad_visible",
                                                          true).toBool());
  m_componentSwitcher->actions().at(Settings::value(APP_CFG_GEN,
                                                    "start_mode",
                                                    0).toInt())->trigger();

  // Reload last position and size of main window.
  QRect screen = qApp->desktop()->screenGeometry();
  resize(Settings::value(APP_CFG_GUI,
                         "window_size",
                         size()).toSize());
  move(Settings::value(APP_CFG_GUI,
                       "window_position",
                       screen.center() - rect().center()).toPoint());
}

void FormMain::hideWindow() {
  hide();
}

void FormMain::showWindow() {
  show();
  activateWindow();
  raise();
}

void FormMain::switchWindowVisibility() {
  if (isVisible() == true) {
    hideWindow();
  }
  else {
    showWindow();
  }
}

void FormMain::saveBeforeQuit() {
  // Save settings.
  Settings::setValue(APP_CFG_GUI, "window_position", pos());
  Settings::setValue(APP_CFG_GUI, "window_size", size());

  // Store variables from calculator into database.
  // This needs to be called here, because QSQL* objects are freed
  // too soon.
  CalculatorWrapper::getInstance().getCalculator()->saveMemoryPlaces();

  // Delete settings manually.
  Settings::deleteSettings();

  qDebug("Exiting the application.");
}

void FormMain::createMenus() {
  // Create switcher for Calculator/Unit Converter/Currency Converter.
  m_componentSwitcher = new QActionGroup(this);
  m_componentSwitcher->setExclusive(true);

  // Add Qonverter modes into switcher.
  m_componentSwitcher->addAction(m_ui->m_actionCalculator);
  m_ui->m_actionCalculator->setData(0);
  m_componentSwitcher->addAction(m_ui->m_actionUnitConverter);
  m_ui->m_actionUnitConverter->setData(1);
  m_componentSwitcher->addAction(m_ui->m_actionCurrencyConverter);
  m_ui->m_actionCurrencyConverter->setData(2);

  // Provide correct icons for buttons.
  m_ui->m_actionQuit->setIcon(QIcon::fromTheme("application-exit",
                                               QIcon(":/graphics/close_app.png")));
  m_ui->m_actionSettings->setIcon(QIcon::fromTheme("preferences-system",
                                                   QIcon(":/graphics/settings_all.png")));
  m_ui->m_actionAboutQonverter->setIcon(QIcon(":/graphics/qonverter.png"));
  m_ui->m_actionAboutQt->setIcon(QIcon::fromTheme("qtlogo",
                                                  QIcon(":/graphics/qt_logo.png")));
  m_ui->m_actionVariables->setIcon(QIcon::fromTheme("document-properties",
                                                    QIcon(":/graphics/variables.png")));

  // Create tray menu
  m_trayMenu = new QMenu(tr("Tray Icon Menu"), this);
  m_trayMenu->addAction(m_ui->m_actionSettings);
  m_trayMenu->addSeparator();
  m_trayMenu->addAction(m_ui->m_actionQuit);
}

void FormMain::createAppConnections() {
  // Make sure clenup is done when application quits.
  connect(qApp, &QCoreApplication::aboutToQuit,
          this, &FormMain::saveBeforeQuit);

  // Make sure application quits when:
  //  a) when last visible window is closed (single-window mode)
  //  b) when quit() is called explicitly (tray-icon mode)
  connect(qApp, &QApplication::lastWindowClosed, [=] () {
    if (QSystemTrayIcon::isSystemTrayAvailable() == false ||
        Settings::value(APP_CFG_GUI, "tray_icon_enabled", false).toBool() == false ||
        Settings::value(APP_CFG_GUI, "tray_icon_action", 0).toBool() == true) {
      qApp->quit();
    }
    // Notify user that Qonverter runs in tray icon mode.
    else if (Settings::value(APP_CFG_GUI, "first_start_tray", true).toBool() == true) {
      Settings::setValue(APP_CFG_GUI, "first_start_tray", false);
      m_trayIcon.data()->showMessage(QString("Qonverter ") + APP_VERSION,
                                     tr("Application is hidden in notification area."));
    }
  });
}

void FormMain::createGuiConnections() {
  // Menu connections.
  connect(m_ui->m_actionQuit, &QAction::triggered, [=] () {
    // Lambda here, because static methods are NOT supported by new s/s syntax.
    qApp->quit();
  });

  connect(m_ui->m_actionAboutQonverter, &QAction::triggered, [=] () {
    FormAbout(this).exec();
  });

  connect(m_ui->m_actionSettings, &QAction::triggered, [=] () {
    if (FormSettings(this).exec() == QDialog::Accepted) {
      // Settings dialog was closed, 'OK' button was hit.
      // Now refresh settings for needed components.

      // Reload skin and style.
      qApp->setStyle(Settings::value(APP_CFG_GUI,
                                     "style",
                                     qApp->style()->objectName()).toString());
      UiFactory::loadSkin(Settings::value(APP_CFG_GUI,
                                          "skin",
                                          "base/plain.qss").toString());

      // Set these after setting style and skin.
      FormCalculator::getInstance()->getHighlighter()->initializeColors();
      FormCalculator::getInstance()->initialize();

      // Refresh tray icon.
      createTrayIcon();
    }
  });

  connect(m_ui->m_actionAboutQt, &QAction::triggered, [=] () {
    // Display custom about Qt dialog.
    // Default dialog is too ugly and big.
    MessageBox::custom(this,
                       tr("About Qt"),
                       tr("<p>This software uses Qt.</p>"
                          "<p>Check <a href=\"http://www.qt-project.org\">www.qt-project.org</a> for more information.</p>"),
                       QMessageBox::Ok,
                       QMessageBox::Ok,
                       QIcon::fromTheme("qtlogo",
                                        QIcon(":/graphics/qt_logo.png")).pixmap(32,
                                                                                32));
  });

  // Display 'Variables' dialog.
  connect(m_ui->m_actionVariables, &QAction::triggered, [=] () {
    FormVariables(this).exec();
  });

  // Switch among modes.
  connect(m_componentSwitcher, &QActionGroup::triggered, [=] (QAction *triggered_action) {
    m_ui->m_stackedWidget->setCurrentIndex(triggered_action->data().toInt());
  });

  // Switch calculator keypad visibility.
  connect(m_ui->m_actionDisplayKeypad, &QAction::triggered, [=] (bool checked) {
    Settings::setValue(APP_CFG_CALC, "keypad_visible", checked);
    FormCalculator::getInstance()->setKeypadVisible(checked);
  });

  // Set focus on correct control, depending on the mode.
  connect(m_ui->m_stackedWidget, &StackedWidget::currentChanged, [=] (int index) {
    switch (index) {
      case 0:
        FormCalculator::getInstance()->getInput()->setFocus(Qt::TabFocusReason);
        m_ui->m_actionDisplayKeypad->setEnabled(true);
        break;
      case 1:
        FormUnitConverter::getInstance()->getInput()->setFocus(Qt::TabFocusReason);
        m_ui->m_actionDisplayKeypad->setEnabled(false);
        break;
      case 2:
        FormCurrencyConverter::getInstance()->getInput()->setFocus(Qt::TabFocusReason);
        m_ui->m_actionDisplayKeypad->setEnabled(false);
        break;
      default:
        break;
    }
  });
}
