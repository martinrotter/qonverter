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

/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>


class QToolButton;

// Based on KLineEdit
// http://api.kde.org/4.x-api/kdelibs-apidocs/kdeui/html/classKLineEdit.html
// and on Lineedit with a clear button
// http://blog.qt.digia.com/blog/2007/06/06/lineedit-with-a-clear-button/
class LineEdit : public QLineEdit {
    Q_OBJECT

  public:
    // Constructors and destructors
    LineEdit(QWidget *parent = 0);
    ~LineEdit();

  public slots:
    // Disables or enables clear button.
    void setClearButtonEnabled(bool enable);

    // Tweak default implementations.
    void setEnabled(bool enable);
    void setReadOnly(bool read_only);

  protected slots:
    void onTextChanged(const QString &new_text);

  protected:
    // Places clear button into the correct position.
    void resizeEvent(QResizeEvent *event);

    void keyPressEvent(QKeyEvent *event);

    // Returns width of QLineEdit frame.
    int frameWidth() const;

  private:
    QToolButton *m_clearButton;
    bool m_clearButtonEnabled;
};


#endif // LIENEDIT_H

