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

#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

#include <QApplication>

#ifndef QT_NO_SHAREDMEMORY
#include <QSharedMemory>
#endif


class SingleApplication : public QApplication {
    Q_OBJECT

  public:
    // Constructors and destructors.
    SingleApplication(int &argc, char *argv[], const QString &unique_key);
    ~SingleApplication();

    // Returns true if this application is already running.
    bool isRunning();

  private:
    bool m_isRunning;
    QString m_uniqueKey;
#ifndef QT_NO_SHAREDMEMORY
    // OS/2 issue here, because no shared memory is implemented on that OS.
    QSharedMemory m_sharedMemory;
#endif
};

#endif // SINGLE_APPLICATION_H
