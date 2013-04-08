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

#include <QApplication>
#include <QTranslator>
#include <QSystemTrayIcon>

// Needed for setting ini file format on Mac OS.
#ifdef Q_OS_MAC
#include <QSettings>
#endif

#include "formwelcome.h"
#include "formmain.h"
#include "debug.h"
#include "defs.h"
#include "settings.h"
#include "singleapplication.h"
#include "uifactory.h"

#ifdef HAVE_DBUS
#include "dbusadaptor.h"
#endif


int main(int argc, char *argv[]) {
  //: FOR TRANSLATOR: Name of language, e.g. English.
  QObject::tr("LANG_NAME");
  //: FOR TRANSLATOR: Abbreviation of language, e.g. en.
  QObject::tr("LANG_ABBREV");
  //: FOR TRANSLATOR: Version of your translation, e.g. 1.0.
  QObject::tr("LANG_VERSION");
  //: FOR TRANSLATOR: Name of translator - optional.
  QObject::tr("LANG_AUTHOR");
  //: FOR TRANSLATOR: Email of translator - optional.
  QObject::tr("LANG_EMAIL");
  //: FOR TRANSLATOR: Website of language - optional.
  QObject::tr("LANG_WEBSITE");

  // Ensure that ini format is used as application settings storage on Mac OS.
#ifdef Q_OS_MAC
  QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

  // Setup debug output system.
  qInstallMessageHandler(Debug::debugHandler);

  // Obtain username for local server key/name
  // The logic behind is to have unique (per session)
  // ID for rssguard to ensure only one app is started
  // Let's suppose it's not possible to start 2 sessions
  // for one user on Windows (username only). But check
  // the DISPLAY on UN*X where it's possible.
  // Examples:
  //		mac: pvanek_/tmp/launch-sXYeRy/org.macosforge.xquartz:0
  //		linux: pvanek_:0
  QString uname;
#ifdef Q_OS_UNIX
  // Mac, GNU/Linux and the other Unices.
  uname = qgetenv("USER");
#endif
#ifdef Q_OS_WIN
  // Windows.
  uname = qgetenv("USERNAME");
#endif
  if (uname.isEmpty() == true) {
    qDebug("Cannot get uname. Using default.");
    uname = APP_LOW_NAME;
  }

  SingleApplication qonverter_app(argc, argv, uname);

  if (Settings::value(APP_CFG_GEN, "one_instance_only", true).toBool() == true &&
      qonverter_app.isRunning() == true) {
    qDebug("Qonverter is already running.");
    return EXIT_SUCCESS;
  }

  // Add 3rd party plugin directory to application PATH variable.
  // This is useful for styles, encoders, ...
  QApplication::addLibraryPath(APP_PLUGIN_PATH);

  // These settings needs to be set before any QSettings object.
  qonverter_app.setApplicationName(APP_NAME);
  qonverter_app.setApplicationVersion(APP_VERSION);
  qonverter_app.setOrganizationName(APP_AUTHORS);
  qonverter_app.setOrganizationDomain(APP_URL);
  qonverter_app.setWindowIcon(QIcon(":/graphics/qonverter.png"));

  // Some Linux-based desktop environments have problem to correctly
  // exit application with setQuitOnLastWindowClosed(true) and
  // existing viible QSystemTrayIcon instance.
  qonverter_app.setQuitOnLastWindowClosed(false);

  // Load style and skin.
  qonverter_app.setStyle(Settings::value(APP_CFG_GUI,
                                         "style",
                                         qonverter_app.style()->objectName()).toString());
  UiFactory::loadSkin(Settings::value(APP_CFG_GUI,
                                      "skin",
                                      "base/plain.qss").toString());

  // Load language and setup locale.
  QRegExp sep("[_|.]+");
  QTranslator qt_translator, app_translator;
  QString lang = Settings::value(APP_CFG_LANG,
                                 "language",
                                 "qonverter_en.qm").toString();
  QString locale_name = lang.section(sep, 1, 1);

  // Try to load selected language file.
  if (app_translator.load(lang, APP_LANG_PATH) == true) {
    qonverter_app.installTranslator(&app_translator);
    qDebug("Language \'%s\' was loaded successfully.", qPrintable(lang));

    // Set up locale.
    qDebug("Setting up '%s' locale.", qPrintable(locale_name));
    QLocale::setDefault(QLocale(locale_name));
  }
  else {
    qDebug("Language \'%s\' wasn't loaded successfully.", qPrintable(lang));

    // Set up locale.
    qDebug("Setting up fallback locale.");
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
  }

  // Setup translation for Qt itself and try to load it.
  if (qt_translator.load(QString("qt_%1.qm").arg(locale_name), APP_LANG_PATH) == false) {
    qDebug("Language for Qt \'%s\' wasn't loaded successfully.",
           qPrintable(QString("qt_%1.qm").arg(locale_name)));
  }
  else {
    qonverter_app.installTranslator(&qt_translator);
    qDebug("Language for Qt \'%s\' was loaded successfully.",
           qPrintable(QString("qt_%1.qm").arg(locale_name)));
  }

  // Create main window and set title.
  FormMain qonverter_window;
  qonverter_window.setWindowTitle(QObject::tr("%1 %2 (as %3)").arg(APP_NAME, APP_VERSION,
                                                                   uname));

#ifdef HAVE_DBUS
  new DBusAdaptor(&qonverter_window);
  QDBusConnection::sessionBus().registerService(DBUS_SERVICE);
  QDBusConnection::sessionBus().registerObject(DBUS_PATH, &qonverter_window);
#endif

  if (Settings::value(APP_CFG_GEN, "first_start", true).toBool() == true) {
    FormWelcome(&qonverter_window).exec();
    Settings::setValue(APP_CFG_GEN, "first_start", false);
  }

  if (Settings::value(APP_CFG_GEN, "start_hidden", false).toBool() == true &&
      QSystemTrayIcon::isSystemTrayAvailable() == true &&
      Settings::value(APP_CFG_GUI, "tray_icon_enabled", false).toBool() == true) {
    // Hide window on startup. But only if tray icon is enabled and available on this operating
    // system and hiding is enabled in settings.
    qonverter_window.hideWindow();
  }
  else {
    qonverter_window.showWindow();
  }

  return qonverter_app.exec();
}
