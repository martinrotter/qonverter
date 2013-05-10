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

#ifndef FLOATINGLABEL_H
#define FLOATINGLABEL_H

#include <QLabel>


class QTimer;

class FloatingLabel : public QLabel {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit FloatingLabel(QWidget *parent = 0);
    ~FloatingLabel();

    // Adjusts position and size of floating label.
    void adjust();

    // Initializes floating label, ie. sets the font.
    void initialize();

  public slots:
    void showText(const QString &text, int duration = 1000);

  private:
    QTimer *m_timer;
};

#endif // FLOATINGLABEL_H
