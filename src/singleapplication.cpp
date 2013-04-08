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

#include <QDebug>

#ifndef HAVE_DBUS
#include <QMessageBox>
#else
#include <QDBusInterface>
#include <QDBusMessage>
#endif

#include "singleapplication.h"
#include "defs.h"
#ifndef HAVE_DBUS
#include "settings.h"
#endif


SingleApplication::SingleApplication(int &argc, char *argv[], const QString &unique_key)
  : QApplication(argc, argv),
    m_isRunning(false),
    m_uniqueKey(unique_key) {

#ifndef QT_NO_SHAREDMEMORY
  m_sharedMemory.setKey(m_uniqueKey);

  if (m_sharedMemory.attach() == true) {
    m_isRunning = true;
#ifdef HAVE_DBUS
    qDebug("Another %s instance is running already. Notifying it.", APP_NAME);
    QDBusInterface dbus(DBUS_SERVICE,
                        DBUS_PATH,
                        DBUS_INTERFACE,
                        QDBusConnection::sessionBus());
    // DBus sometimes doesn't react well so application doesn't start.
    // This happens particularly if Qonverter is started/shut down repeatedly and fast.
    if (dbus.isValid() == false) {
      qWarning("DbusCall: QDBusInterface is invalid.");
      return;
    }
    QDBusMessage msg = dbus.call("raise");
    if (msg.errorName().isEmpty() == false) {
      qDebug("%s", qPrintable(msg.errorMessage()));
    }
#else
    if (Settings::value(APP_CFG_GEN, "one_instance_only", true).toBool() == true) {
      QMessageBox::warning(0,
                           tr("%1 is running already").arg(APP_NAME),
                           tr("Another instance of %1 is already running.").arg(APP_NAME));
    }
#endif
  }
  else {
    m_isRunning = false;
    // Create shared memory.
    if (m_sharedMemory.create(1) == false) {
      qDebug("Unable to create single instance.");
      return;
    }
  }
#else
  qDebug("QT_NO_SHAREDMEMORY is defined. There can run more instances of applcation. This usually concerns OS/2.");
#endif
}

SingleApplication::~SingleApplication() {
#ifndef QT_NO_SHAREDMEMORY
  qDebug("Detaching from shared memory.");
#endif
}

bool SingleApplication::isRunning() {
  return m_isRunning;
}
