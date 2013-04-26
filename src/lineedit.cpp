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

#include <QToolButton>
#include <QStyle>
#include <QLocale>
#include <QKeyEvent>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <winuser.h>
#endif

#include "lineedit.h"


LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent) {
  m_clearButton = new QToolButton(this);

  int frame_width = frameWidth();
  m_clearButton->setIcon(QIcon::fromTheme("edit-clear",
                                          QIcon(":/graphics/clear_contents.png")));
  m_clearButton->setIconSize(QSize(sizeHint().height() - 4 * frame_width,
                                   sizeHint().height() - 4 * frame_width));
  m_clearButton->setCursor(Qt::ArrowCursor);
  m_clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
  m_clearButton->setToolTip(tr("Remove contents of this text box."));
  m_clearButton->hide();
  m_clearButtonEnabled = true;

  // Create necessary connections.
  connect(m_clearButton, &QToolButton::clicked, this, &LineEdit::clear);
  connect(this, &LineEdit::textChanged, this, &LineEdit::onTextChanged);

  // Add extra padding to the right of the line edit. It looks better.
  setStyleSheet(QString("LineEdit { padding-right: %1px; }").arg(m_clearButton->sizeHint().width() +
                                                                 frame_width + 1));
  QSize min_size_hint = minimumSizeHint();
  setMinimumSize(qMax(min_size_hint.width(),
                      m_clearButton->sizeHint().height() + frame_width),
                 qMax(min_size_hint.height(),
                      m_clearButton->sizeHint().height() + frame_width));
}

LineEdit::~LineEdit() {
  delete m_clearButton;
}

void LineEdit::onTextChanged(const QString &new_text) {
  // If line edit is not read only (or not enabled) and clear button
  // is enabled, then make sure it's displayed.
  if (isReadOnly() == false && isEnabled() == true && m_clearButtonEnabled == true) {
    m_clearButton->setVisible(new_text.isEmpty() == false);
  }
  else {
    m_clearButton->setVisible(false);
  }
}

void LineEdit::setClearButtonEnabled(bool enable) {
  m_clearButtonEnabled = enable;
}

void LineEdit::setEnabled(bool enable) {
  QLineEdit::setEnabled(enable);
  onTextChanged(text());
}

void LineEdit::setReadOnly(bool read_only) {
  QLineEdit::setReadOnly(read_only);
  onTextChanged(text());
}

int LineEdit::frameWidth() const {
  return style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
}

void LineEdit::keyPressEvent(QKeyEvent *event) {
  // Make sure that commas are replaced by dots
  // as decimal separator.
#if defined(Q_OS_WIN)
  if (event->key() == Qt::Key_Comma &&
      event->nativeVirtualKey() == VK_DECIMAL) {
    insert(".");
    event->ignore();
    return;
  }
#endif

  // React as normal line edit.
  QLineEdit::keyPressEvent(event);
}

void LineEdit::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);

  // Place clear button correctly, according to size of line edit.
  QSize sz = m_clearButton->sizeHint();
  m_clearButton->move(rect().right()  - sz.width(),
                      rect().bottom() - sz.height() + frameWidth());
}
