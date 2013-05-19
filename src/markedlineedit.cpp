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

#include <QToolButton>
#include <QEvent>

#include "markedlineedit.h"
#include "toolbutton.h"
#include "balloontip.h"


MarkedLineEdit::MarkedLineEdit(QWidget *parent)
  : LineEdit(parent), m_statusDisplayLength(-1) {
  m_btnStatus = new ToolButton(this);

  // Prepare icons for valid and invalid state.
  m_iconOk = QIcon::fromTheme("dialog-ok-apply",
                              QIcon(":/graphics/valid.png"));
  m_iconError = QIcon::fromTheme("edit-delete",
                                 QIcon(":/graphics/invalid.png"));

  // Set the icon to have the same dimensions as is the height of line edit.
  QSize sz = sizeHint();
  m_btnStatus->setIconSize(QSize(sz.height(),
                                 sz.height()));

  // Set correct cursor.
  m_btnStatus->setCursor(Qt::ArrowCursor);

  // Make sure that mark icon has no border and padding.
  m_btnStatus->setStyleSheet("QToolButton { border: none; padding: 0px; }");

  // Forward signal from mark icon.
  connect(m_btnStatus, &ToolButton::hovered, this, &MarkedLineEdit::showStatus);

  // Make room at the right end of line edit for mark button.
  // Make sure there is extra margin between line edit and mark icon.
  // There is very UNPLEASANT issue in Qt. Default CSS properties of child
  // class override properties of parent class.
  int frame_width = frameWidth();
  setStyleSheet(QString("%1 %2").arg(QString(MARKED_LINE_EDIT_MARGIN).arg(sz.height() + frame_width),
                                     QString(LINE_EDIT_PADDING).arg(m_btnClear->sizeHint().width() + frameWidth() + 1)));
}

MarkedLineEdit::~MarkedLineEdit() {
  delete m_btnStatus;
}

bool MarkedLineEdit::event(QEvent *e) {
  switch (e->type()) {
    case QEvent::ToolTip: {
      QPoint distance = QCursor::pos() - mapToGlobal(m_btnStatus->pos());
      if (sqrt((distance.x() ^ 2) + (distance.y() ^ 2)) < 10) {
        return true;
      }
      break;
    }
    default:
      break;
  }

  return LineEdit::event(e);
}

void MarkedLineEdit::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);

  QSize sz_clear = m_btnClear->sizeHint();
  QSize sz_mark = m_btnStatus->sizeHint();
  QSize sz_lineedit = sizeHint();

  int frame_width = frameWidth();

  // We need to move the clear button more to the left.
  m_btnClear->move(rect().right() - sz_lineedit.height() - sz_clear.width() - frame_width,
                   rect().bottom() - sz_clear.height() + frame_width);

  // Move mark button to the right of line edit.
  m_btnStatus->move(rect().right() - sz_lineedit.height(),
                    rect().bottom() - sz_mark.height() + 2 * frame_width);
}

void MarkedLineEdit::setEnabled(bool enable) {
  LineEdit::setEnabled(enable);

  int frame_width = frameWidth();

  if (enable) {
    setStyleSheet(QString("%1 %2").arg(QString(MARKED_LINE_EDIT_MARGIN).arg(sizeHint().height() + frame_width),
                                       QString(LINE_EDIT_PADDING).arg(m_btnClear->sizeHint().width() + frame_width + 1)));
  }
  else {
    setStyleSheet(QString("%1 %2").arg(QString(MARKED_LINE_EDIT_MARGIN).arg(sizeHint().height() + frame_width),
                                       QString(LINE_EDIT_PADDING).arg(0)));
  }

  onTextChanged(text());
}

void MarkedLineEdit::setStatusDisplayLength(int length) {
  m_statusDisplayLength = length;
}

void MarkedLineEdit::setStatusText(const QString &text) {
  m_statusText = text;
}

void MarkedLineEdit::hideStatus() {
  BalloonTip::hideBalloon();
}

void MarkedLineEdit::showStatus() {
  BalloonTip::showBalloon(m_statusText,
                          mapToGlobal(QPoint(22, sizeHint().height() - 3)),
                          m_statusDisplayLength);
}

MarkedLineEdit::Status MarkedLineEdit::icon() {
  return m_status;
}

void MarkedLineEdit::setIcon(Status icon) {
  m_status = icon;

  switch (m_status) {
    case OK:
      m_btnStatus->setIcon(m_iconOk);
      break;
    case ERROR:
      m_btnStatus->setIcon(m_iconError);
      break;
    default:
      break;
  }
}
