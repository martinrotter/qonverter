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

#ifndef BALOONTIP_H
#define BALOONTIP_H

#include <QWidget>


class QLabel;
class QEvent;
class QTimerEvent;
class QMouseEvent;

class BalloonTip : public QWidget
{
    Q_OBJECT
  public:
    static void showBalloon(const QString& msg, const QPoint& pos,
                            int timeout, bool showArrow = true);
    static void hideBalloon();
    static bool isBalloonVisible();
    static void erase();

    bool eventFilter(QObject *o, QEvent *e);

  private:
    BalloonTip();
    ~BalloonTip();

    void balloon(const QPoint&, int, bool);

  protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *e);

  private:
    QLabel *msgLabel;
    QPixmap pixmap;
    int timerId;

    static BalloonTip *s_instance;
};

#endif // BALOONTIP_H
