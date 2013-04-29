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

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>


class Database {
  public:
    // Establishes new SQLite connection and returns its handle.
    static QSqlDatabase addDatabaseConnection(const QString &name);

    // Returns handle of an existing connection.
    static QSqlDatabase getDatabaseConnection(const QString &name);

    // Removes already existing SQLite connection.
    static void removeDatabaseConnection(const QString &name);

    static void removeAllConnections();

  private:
    // Returns standard path to store application database file.
    static QString getDatabasePath();

    // Initalizes database with initial data - creates necessary tables.
    static void initializeDatabase(QSqlDatabase &db, QSqlQuery &q);
};

#endif // DATABASE_H
