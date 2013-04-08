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

#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QStackedWidget>


class StackedWidget : public QStackedWidget {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit StackedWidget(QWidget *parent = 0);

    // Reimplemented from QStackedWidget.
    void setCurrentIndex(int index);
};

#endif // STACKEDWIDGET_H
