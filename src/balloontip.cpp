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
    Copyright 2012 Nokia Corporation
*/


#include <QLabel>
#include <QStyle>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLayout>
#include <QBitmap>
#include <QPainter>
#include <QEvent>
#include <QApplication>

#include "balloontip.h"
#include "defs.h"
#include "settings.h"


BalloonTip *BalloonTip::s_instance = nullptr;

void BalloonTip::showBalloon(const QString& message, const QPoint& pos,
                             int timeout, bool showArrow) {
  hideBalloon();

  if (message.isEmpty()) {
    return;
  }

  if (s_instance == nullptr) {
    s_instance = new BalloonTip();
  }

  s_instance->m_lblMessage->setText(message);

  if (timeout < 0) {
    // Fetch show duration from seetings
    // instead of constant 10 sec value.
    timeout = Settings::value(APP_CFG_CALC, "length_error", 6000).toInt();
  }
  s_instance->balloon(pos, timeout, showArrow);
}

void BalloonTip::hideBalloon() {
  if (s_instance == nullptr) {
    return;
  }
  s_instance->hide();
}

bool BalloonTip::isBalloonVisible() {
  return s_instance != nullptr && s_instance->isVisible();
}

void BalloonTip::erase() {
  if (s_instance != nullptr) {
    s_instance->hideBalloon();
    delete s_instance;

    qDebug("Removed global BalloonTip.");
  }
  else {
    qDebug("No need to remove global BalloonTip. It is not created.");
  }
}

bool BalloonTip::eventFilter(QObject *o, QEvent *e) {
  Q_UNUSED(o);

  switch (e->type()) {
#ifdef Q_OS_MAC
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      int key = static_cast<QKeyEvent *>(e)->key();
      Qt::KeyboardModifiers mody = static_cast<QKeyEvent *>(e)->modifiers();
      if (!(mody & Qt::KeyboardModifierMask)
          && key != Qt::Key_Shift && key != Qt::Key_Control
          && key != Qt::Key_Alt && key != Qt::Key_Meta) {
        close();
      }
      break;
    }
#endif
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    case QEvent::Wheel:
      close();
      break;
    default:
      break;
  }
  return false;
}

BalloonTip::BalloonTip() : QWidget(0, Qt::ToolTip), m_timerId(-1) {
  // Install global event filter for this widget.
  qApp->installEventFilter(this);

  m_lblMessage = new QLabel(this);
  m_lblMessage->installEventFilter(this);
  m_lblMessage->setTextFormat(Qt::PlainText);
  m_lblMessage->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Smart size for the message label.
  int limit = QApplication::desktop()->availableGeometry(m_lblMessage).size().width() / 3;

  // Force word wrap and reasonable maximum width for balloon.
  m_lblMessage->setMaximumWidth(limit);
  m_lblMessage->setWordWrap(true);

  // Compose all elements into grid layout.
  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(m_lblMessage, 0, 0, -1, -1);
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->setMargin(3);
  setLayout(layout);

  // Set tooltip-like palette.
  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor(0xff, 0xff, 0xe1));
  pal.setColor(QPalette::WindowText, Qt::black);
  setPalette(pal);

  // Set window opacity.
  // This actually afffects just operating systems
  // with hardware accelerated environments or window opacity enabled.
  setWindowOpacity(0.85);
}

BalloonTip::~BalloonTip() {
  // Unregister global filter for this widget.
  qApp->removeEventFilter(this);

  // Ensure that singleton is set to nullptr.
  s_instance = nullptr;
}

void BalloonTip::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.drawPixmap(rect(), m_pixmap);
}

void BalloonTip::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
}

void BalloonTip::balloon(const QPoint& pos, int msecs, bool showArrow) {
  QRect scr = QApplication::desktop()->screenGeometry(pos);
  QSize sh = sizeHint();
  const int border = 1;
  const int ah = 18, ao = 18, aw = 18, rc = 7;
  bool arrow_at_top = (pos.y() + sh.height() + ah < scr.height());
  bool arrow_at_left = (pos.x() + sh.width() - ao < scr.width());
  setContentsMargins(border + 3,
                     border + (arrow_at_top ? ah : 0) + 2,
                     border + 3,
                     border + (arrow_at_top ? 0 : ah) + 2);
  updateGeometry();
  sh  = sizeHint();

  int ml, mr, mt, mb;
  QSize sz = sizeHint();
  if (!arrow_at_top) {
    ml = mt = 0;
    mr = sz.width() - 1;
    mb = sz.height() - ah - 1;
  }
  else {
    ml = 0;
    mt = ah;
    mr = sz.width() - 1;
    mb = sz.height() - 1;
  }

  QPainterPath path;
#if defined(QT_NO_XSHAPE) && defined(Q_OS_X11)
  // XShape is required for setting the mask, so we just
  // draw an ugly square when its not available
  path.moveTo(0, 0);
  path.lineTo(sz.width() - 1, 0);
  path.lineTo(sz.width() - 1, sz.height() - 1);
  path.lineTo(0, sz.height() - 1);
  path.lineTo(0, 0);
  move(qMax(pos.x() - sz.width(), scr.left()), pos.y());
#else
  path.moveTo(ml + rc, mt);
  if (arrow_at_top && arrow_at_left) {
    if (showArrow) {
      path.lineTo(ml + ao, mt);
      path.lineTo(ml + ao, mt - ah);
      path.lineTo(ml + ao + aw, mt);
    }

    move(qMax(pos.x() - ao, scr.left() + 2), pos.y());
  }
  else if (arrow_at_top && !arrow_at_left) {
    if (showArrow) {
      path.lineTo(mr - ao - aw, mt);
      path.lineTo(mr - ao, mt - ah);
      path.lineTo(mr - ao, mt);
    }

    move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2), pos.y());
  }
  path.lineTo(mr - rc, mt);
  path.arcTo(QRect(mr - rc*2, mt, rc*2, rc*2), 90, -90);
  path.lineTo(mr, mb - rc);
  path.arcTo(QRect(mr - rc*2, mb - rc*2, rc*2, rc*2), 0, -90);
  if (!arrow_at_top && !arrow_at_left) {
    if (showArrow) {
      path.lineTo(mr - ao, mb);
      path.lineTo(mr - ao, mb + ah);
      path.lineTo(mr - ao - aw, mb);
    }

    move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2),
         pos.y() - sh.height());
  }
  else if (!arrow_at_top && arrow_at_left) {
    if (showArrow) {
      path.lineTo(ao + aw, mb);
      path.lineTo(ao, mb + ah);
      path.lineTo(ao, mb);
    }

    move(qMax(pos.x() - ao, scr.x() + 2), pos.y() - sh.height());
  }
  path.lineTo(ml + rc, mb);
  path.arcTo(QRect(ml, mb - rc*2, rc*2, rc*2), -90, -90);
  path.lineTo(ml, mt + rc);
  path.arcTo(QRect(ml, mt, rc*2, rc*2), 180, -90);

  // Set the mask.
  QBitmap bitmap = QBitmap(sizeHint());
  bitmap.fill(Qt::color0);
  QPainter painter1(&bitmap);
  painter1.setPen(QPen(Qt::color1, border));
  painter1.setBrush(QBrush(Qt::color1));
  painter1.drawPath(path);
  setMask(bitmap);
#endif

  // Draw the border.
  m_pixmap = QPixmap(sz);
  QPainter painter2(&m_pixmap);
  painter2.setPen(QPen(palette().color(QPalette::Window).darker(160), border));
  painter2.setBrush(palette().color(QPalette::Window));
  painter2.drawPath(path);

  if (msecs > 0) {
    m_timerId = startTimer(msecs);
  }
  show();
}

void BalloonTip::mousePressEvent(QMouseEvent *event) {
  Q_UNUSED(event);

  close();
}

void BalloonTip::timerEvent(QTimerEvent *event) {
  if (event->timerId() == m_timerId) {
    killTimer(m_timerId);

    if (!underMouse()) {
      close();
    }
    return;
  }

  QWidget::timerEvent(event);
}
