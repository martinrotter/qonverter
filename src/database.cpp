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

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>
#include <QVariant>

#include "defs.h"
#include "database.h"


QSqlDatabase Database::addDatabaseConnection(const QString &name) {
  QString db_path = QDir::toNativeSeparators(getDatabasePath());
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", name);
  QString folder = db_path.left(db_path.lastIndexOf(QDir::separator()) + 1);

  // Check if SQLite driver is available.
  // It it isn't, then variables cannot be persistently
  // stored.
  if (!db.isValid()) {
    qFatal("QSQLITE database server was NOT found. Make sure that application and its dependencies are installed correctly.");
    return db;
  }

  db.setDatabaseName(db_path);
  qDebug("Opening database '%s'.",
         qPrintable(QDir::toNativeSeparators(db.databaseName())));

  // Ensure that path structure leading to database file storage exists.
  QDir().mkpath(folder);

  if (db.open()) {
    // Database is opened. Setup necessary pragmas.
    db.exec("PRAGMA synchronous = OFF");
    db.exec("PRAGMA journal_mode = MEMORY");
    db.exec("PRAGMA count_changes = OFF");
    db.exec("PRAGMA temp_store = MEMORY");

    // Execute test query.
    // This query checks for existence of tables and schema_version key.
    QSqlQuery q = db.exec("SELECT q_information.value FROM q_information WHERE q_information.key = 'schema_version'");

    if (q.lastError().isValid()) {
      // Query returns error.
      qWarning("Error occurred. Database is not fully initialized. Initializing now.");
      initializeDatabase(db, q);
    }
    else {
      // Query was successfull, print shema_version.
      q.next();
      qDebug("Database connection '%s' to file %s seems to be loaded.",
             qPrintable(name),
             qPrintable(QDir::toNativeSeparators(db.databaseName())));
      qDebug("Version of database schema is '%s'.",
             qPrintable(q.value(0).toString()));
    }
    // Free resources.
    q.finish();
  }
  else {
    // Database is NOT opened.
    qFatal("Database was NOT opened. Error occurred: %s",
           qPrintable(db.lastError().databaseText()));
  }
  return db;
}

void Database::initializeDatabase(QSqlDatabase &db, QSqlQuery &q) {
  // Open file with initialization SQL code.
  QFile file_init(":/database/init.sql");
  file_init.open(QIODevice::ReadOnly | QIODevice::Text);

  // Split file into statements and begin transaction.
  QStringList statements = QString(file_init.readAll()).split("-- !\n");
  db.exec("BEGIN TRANSACTION");

  // Execute each statement.
  foreach(QString i, statements) {
    q = db.exec(i);

    // If error occurrs, then exit initialization.
    if (q.lastError().isValid() && q.lastError().number() != -1) {
      db.exec("COMMIT");
      qWarning("Initialization of database '%s' failed.",
               qPrintable(QDir::toNativeSeparators(db.databaseName())));
      return;
    }
  }

  db.exec("COMMIT");
  qWarning("Database backend is ready now.");
}

QSqlDatabase Database::getDatabaseConnection(const QString &name) {
  return QSqlDatabase::database(name);
}

void Database::removeDatabaseConnection(const QString &name) {
  QSqlDatabase::removeDatabase(name);
  qDebug("Removed database connection %s.", qPrintable(name));
}

void Database::removeAllConnections() {
  foreach (QString connection, QSqlDatabase::connectionNames()) {
    Database::removeDatabaseConnection(connection);
  }
}

QString Database::getDatabasePath() {
  return QDir::homePath() + QDir::separator() + APP_LOW_H_NAME +
      QDir::separator() + APP_DB_PATH;
}
