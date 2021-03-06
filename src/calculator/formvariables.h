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

#ifndef FORMVARIABLES_H
#define FORMVARIABLES_H

#include <QDialog>

#include "ui_formvariables.h"


namespace Ui {
  class FormVariables;
}

class ConstantsModel;

class FormVariables : public QDialog {
    Q_OBJECT

  public:
    // Constructors and detructors.
    explicit FormVariables(QWidget *parent = 0);
    ~FormVariables();

    static FormVariables *getInstance();

  protected:
    void keyPressEvent(QKeyEvent *e);

  protected slots:
    // Displays variable add dialog.
    void addVariable();

    // Displays variable edit dialog for current variable.
    void editVariable();

    // Deletes currently selected variable and reloads the list.
    void deleteVariable();

    void enableVariableButtons(const QModelIndex &current_index);

  private:
    Ui::FormVariables *m_ui;
    ConstantsModel *m_model;
    ConstantsFilterModel *m_filterModel;

    static FormVariables *s_instance;
};

#endif // FORMVARIABLES_H
