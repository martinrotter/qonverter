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

#ifndef FORMABOUT_H
#define FORMABOUT_H

#include <QDialog>

#include "ui_formabout.h"
#include "defs.h"


namespace Ui {
  class FormAbout;
}

class FormAbout : public QDialog {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit FormAbout(QWidget *parent);
    ~FormAbout();

  private:
    Ui::FormAbout *m_ui;
};


#endif // FORMABOUT_H
