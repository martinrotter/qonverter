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

#include "debug.h"
#include "defs.h"

#include <cstdio>

#ifndef QT_NO_DEBUG_OUTPUT
#define DEBUG_OUTPUT_WORKER(type_string, placement, message) \
  fprintf(stderr, "[%s] %s (%s, line %d) : %s\n", \
  APP_LOW_NAME, \
  type_string, \
  placement.file, \
  placement.line, \
  qPrintable(message));
#endif

void Debug::debugHandler(QtMsgType type,
                         const QMessageLogContext &placement,
                         const QString &message) {
#ifndef QT_NO_DEBUG_OUTPUT
  switch (type) {
    case QtDebugMsg:
      DEBUG_OUTPUT_WORKER("INFO", placement, message);
      break;
    case QtWarningMsg:
      DEBUG_OUTPUT_WORKER("WARNING", placement, message);
      break;
    case QtCriticalMsg:
      DEBUG_OUTPUT_WORKER("CRITICAL", placement, message);
      break;
    case QtFatalMsg:
      DEBUG_OUTPUT_WORKER("FATAL", placement, message);
      qApp->exit(EXIT_FAILURE);
  }
#endif
}
