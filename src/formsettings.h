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

#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QDialog>

#include "ui_formsettings.h"
#include "calculatorhighlighter.h"


namespace Ui {
  class FormSettings;
}

class CalculatorHighlighter;

struct TemporarySettings {
    QFont m_calcFont;
    QFont m_otfFont;
    QList<QColor> m_calcColors;
};

class FormSettings : public QDialog {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit FormSettings(QWidget *parent = 0);
    ~FormSettings();

  protected slots:
    // Saves settings into global configuration.
    void saveSettings();

  protected:
    // Loads/saves settings from/to all categories.
    void loadGeneral();
    void saveGeneral();

    void loadInterface();
    void saveInterface();

    void loadCalculator();
    void saveCalculator();

    void loadLanguages();
    void saveLanguages();

    // Sets newly selected color as background color for this button.
    void setButtonColor(QPushButton *button, const QColor &color);

    // Saves newly selected color.
    void saveColor(const CalculatorHighlighter::HighlighterRole &role, const QColor &color);

  private:
    Ui::FormSettings *m_ui;

    // Highlighter for previewing calculator colors.
    CalculatorHighlighter *m_highlighter;

    // Temporary setting holder. Its needed for some actions.
    TemporarySettings m_tempSettings;
};

#endif // FORMSETTINGS_H
