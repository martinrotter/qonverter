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

  s_instance->msgLabel->setText(message);

  if (timeout < 0) {
    // Fetch show duration from seetings
    // instead of constant 10 sec value.
    timeout = Settings::value(APP_CFG_CALC, "length_error", 3000).toInt();
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
    s_instance = 0;

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
      /*
    case QEvent::Leave:
      close();
      break;
      */
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
      /*
    case QEvent::MouseMove:
      if (o == this && !rect().isNull() &&
          !rect().contains(static_cast<QMouseEvent*>(e)->pos())) {
        close();
      }
      */
    default:
      break;
  }
  return false;
}

BalloonTip::BalloonTip() : QWidget(0, Qt::ToolTip), timerId(-1) {
  setAttribute(Qt::WA_DeleteOnClose);

  qApp->installEventFilter(this);

  msgLabel = new QLabel(this);
#ifdef Q_OS_WINCE
  f.setBold(false);
  msgLabel->setFont(f);
#endif
  msgLabel->installEventFilter(this);
  msgLabel->setTextFormat(Qt::PlainText);
  msgLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Smart size for the message label.
#ifdef Q_OS_WINCE
  int limit = QApplication::desktop()->availableGeometry(msgLabel).size().width() / 2;
#else
  int limit = QApplication::desktop()->availableGeometry(msgLabel).size().width() / 3;
#endif
  if (msgLabel->sizeHint().width() > limit) {
    msgLabel->setWordWrap(true);

    if (msgLabel->sizeHint().width() > limit) {
      /*
      msgLabel->d_func()->ensureTextControl();
      if (QTextControl *control = msgLabel->d_func()->control) {
        QTextOption opt = control->document()->defaultTextOption();
        opt.setWrapMode(QTextOption::WrapAnywhere);
        control->document()->setDefaultTextOption(opt);
      }
      */
    }
#ifdef Q_OS_WINCE
    // Make sure that the text isn't wrapped "somewhere" in the balloon widget
    // in the case that we have a long title label.
    setMaximumWidth(limit);
#else
    // Here we allow the text being much smaller than the balloon widget
    // to emulate the weird standard windows behavior.
    msgLabel->setFixedSize(limit, msgLabel->heightForWidth(limit));
#endif
  }

  QGridLayout *layout = new QGridLayout(this);

  layout->addWidget(msgLabel, 0, 0, -1, -1);
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->setMargin(3);
  setLayout(layout);

  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor(0xff, 0xff, 0xe1));
  pal.setColor(QPalette::WindowText, Qt::black);
  setPalette(pal);

  setWindowOpacity(0.83);
}

BalloonTip::~BalloonTip() {
  qApp->removeEventFilter(this);
  s_instance = nullptr;
}

void BalloonTip::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.drawPixmap(rect(), pixmap);
}

void BalloonTip::resizeEvent(QResizeEvent *ev) {
  QWidget::resizeEvent(ev);
}

void BalloonTip::balloon(const QPoint& pos, int msecs, bool showArrow) {
  QRect scr = QApplication::desktop()->screenGeometry(pos);
  QSize sh = sizeHint();
  const int border = 1;
  const int ah = 18, ao = 18, aw = 18, rc = 7;
  bool arrowAtTop = (pos.y() + sh.height() + ah < scr.height());
  bool arrowAtLeft = (pos.x() + sh.width() - ao < scr.width());
  setContentsMargins(border + 3,
                     border + (arrowAtTop ? ah : 0) + 2,
                     border + 3,
                     border + (arrowAtTop ? 0 : ah) + 2);
  updateGeometry();
  sh  = sizeHint();

  int ml, mr, mt, mb;
  QSize sz = sizeHint();
  if (!arrowAtTop) {
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
  if (arrowAtTop && arrowAtLeft) {
    if (showArrow) {
      path.lineTo(ml + ao, mt);
      path.lineTo(ml + ao, mt - ah);
      path.lineTo(ml + ao + aw, mt);
    }

    move(qMax(pos.x() - ao, scr.left() + 2), pos.y());
  }
  else if (arrowAtTop && !arrowAtLeft) {
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
  if (!arrowAtTop && !arrowAtLeft) {
    if (showArrow) {
      path.lineTo(mr - ao, mb);
      path.lineTo(mr - ao, mb + ah);
      path.lineTo(mr - ao - aw, mb);
    }

    move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2),
         pos.y() - sh.height());
  }
  else if (!arrowAtTop && arrowAtLeft) {
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
  pixmap = QPixmap(sz);
  QPainter painter2(&pixmap);
  painter2.setPen(QPen(palette().color(QPalette::Window).darker(160), border));
  painter2.setBrush(palette().color(QPalette::Window));
  painter2.drawPath(path);

  if (msecs > 0) {
    timerId = startTimer(msecs);
  }
  show();
}

void BalloonTip::mousePressEvent(QMouseEvent *event) {
  Q_UNUSED(event);

  close();
}

void BalloonTip::timerEvent(QTimerEvent *e) {
  if (e->timerId() == timerId) {
    killTimer(timerId);

    if (!underMouse()) {
      close();
    }
    return;
  }

  QWidget::timerEvent(e);
}
