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

#include <QToolTip>
#include <QTimer>
#include <QApplication>

#include "floatinglabel.h"
#include "settings.h"
#include "defs.h"


// Parent is used for placement of the label.
FloatingLabel::FloatingLabel(QWidget *parent) : QLabel(parent) {
  setVisible(false);
  setPalette(QToolTip::palette());
  setAutoFillBackground(true);
  setFrameShape(QFrame::Box);
  setWordWrap(true);

  m_timer = new QTimer(this);
  m_timer->setSingleShot(true);

  connect(m_timer, &QTimer::timeout, this, &FloatingLabel::hide);
}

FloatingLabel::~FloatingLabel() {
  m_timer->stop();
  delete m_timer;
}

void FloatingLabel::adjust() {
  // Make label big enough to contain all contents.
  adjustSize();

  // Make sure it's width as parent widget.
  setFixedWidth(parentWidget()->width());

  // Move it to the bottom of parent widget.
  move(pos().x(),
       parentWidget()->pos().y() + parentWidget()->height() - height());
}

void FloatingLabel::initialize() {
  setFont(Settings::value(APP_CFG_CALC,
                          "on_the_fly_font",
                          qApp->font()).value<QFont>());
}

void FloatingLabel::showText(const QString &text, int duration) {
  // Set text, adjust size and position and show.
  setText(text);
  adjust();
  show();

  // Start the interval for showing one piece of text.
  m_timer->stop();
  m_timer->start(duration);
}
