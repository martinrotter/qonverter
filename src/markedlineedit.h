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

#ifndef MARKEDLINEEDIT_H
#define MARKEDLINEEDIT_H

#define MARKED_LINE_EDIT_MARGIN "MarkedLineEdit { margin-right: %1px; }"

#include "lineedit.h"


class ToolButton;

class MarkedLineEdit : public LineEdit {
    Q_OBJECT

  public:
    enum Status {
      OK,
      ERROR
    };

    explicit MarkedLineEdit(QWidget *parent = 0);
    ~MarkedLineEdit();

  public slots:
    Status icon();
    void setIcon(Status icon);

    void setStatusText(const QString &text);
    void setStatusDisplayLength(int length);

    void showStatus();
    void hideStatus();

    void setEnabled(bool enable);

  protected:
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *e);

  signals:
    void statusIconHovered(const MarkedLineEdit::Status &status);

  private:
    QIcon m_iconOk;
    QIcon m_iconError;

    ToolButton *m_btnStatus;
    int m_statusDisplayLength;

    Status m_status;
    QString m_statusText;
};

#endif // MARKEDLINEEDIT_H
