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

#ifndef CALCULATORINPUT_H
#define CALCULATORINPUT_H

#include <QPlainTextEdit>


class QCompleter;

// TODO: Use completer for all line edits which do use computations.
class CalculatorInput : public QPlainTextEdit {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit CalculatorInput(QWidget *parent = 0);

    ~CalculatorInput();

    // Initializes look of this input.
    void initialize();

    // Returns text which is now located "under" the cursor.
    QString textUnderCursor() const;

  protected:
    void keyPressEvent(QKeyEvent *e);

    // Parenthesis matchers.
    bool matchLeftPar(QTextBlock current_block, int index, int num_right_par,
                      QChar lft = '(', QChar rgt = ')');
    bool matchRightPar(QTextBlock current_block, int index, int num_left_par,
                       QChar lft = '(', QChar rgt = ')');
    void createParSelection(int pos);

  signals:
    // Emitted if user hits ENTER.
    void submitted(QString text);

    // Emit if user edits contents.
    void textEdited(QString text);

  public slots:
    // Needed because of special calculator initialization.
    // Sets the singleton calculator model as data source for auto-completion.
    void setCompleterModel();

  protected slots:
    // Goes through contents and highlights matching parenthesises.
    void matchPar();

    // Inserts selected completion into text box.
    void insertCompletion(const QString& completion);

  private:
    QCompleter *m_completer;
};

#endif // CALCULATORINPUT_H
