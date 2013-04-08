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

#include <QDesktopServices>
#include <QUrl>
#include <QDesktopWidget>

#include "formwelcome.h"
#include "defs.h"


FormWelcome::FormWelcome(QWidget *parent) : QDialog(parent), m_ui(new Ui::FormWelcome) {
  m_ui->setupUi(this);

  // Set flags.
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);

  // Move the dialog into the middle of the screen.
  QRect screen = qApp->desktop()->screenGeometry();
  move(screen.center() - rect().center());

  // Make sure that clicked hyperlinks are opened in defult web browser.
  connect(m_ui->m_lblInfo, &QLabel::linkActivated, [=] (const QString &link) {
    QDesktopServices::openUrl(QUrl(link));
  });
}

FormWelcome::~FormWelcome() {
  delete m_ui;
}
