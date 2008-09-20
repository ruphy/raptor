/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorscrollbutton.h"

#include <plasma/svg.h>

#include <QRectF>
#include <QPainter>
#include <QPaintEvent>
#include <QTimeLine>

#include <KDebug>

class RaptorScrollButton::Private
{
    public:
        Private(Side s)
          : side(s),
            frame(0)
        {}
        ~Private()
        {}

        Side side;
        Plasma::Svg * svg;
        QTimeLine * timeLine;
        int frame;
};
RaptorScrollButton::RaptorScrollButton(Side side, QWidget * parent)
  : QPushButton(parent),
    d(new Private(side))
{
    d->svg = new Plasma::Svg(this);
    d->svg->setImagePath("widgets/raptorarrows");
    setAttribute(Qt::WA_NoSystemBackground);
    repaint();
    installEventFilter(this);
    d->timeLine = new QTimeLine(250, this);//TODO: Move to private class
    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(animatePaint(int)));
}

RaptorScrollButton::~RaptorScrollButton()
{
}

void RaptorScrollButton::paintEvent(QPaintEvent * event)
{
    kDebug();
    QPainter p(this);
    QRectF r(event->rect());
    r.setSize(QSizeF(size().width() * 0.7 + d->frame, size().height() * 0.6 + d->frame));
    r.moveCenter(QPointF(size().width() / 2, size().height() / 2));
    if (d->side == Right) {
       d->svg->paint(&p, r, "rightarrow");
    }
    else {
       d->svg->paint(&p, r, "leftarrow");
    }
}

bool RaptorScrollButton::eventFilter(QObject * watched, QEvent * event)
{
    switch(event->type())
    {
        case QEvent::HoverEnter:
            d->timeLine->setFrameRange(0, 20);
            d->timeLine->start();
            return false;
        case QEvent::HoverLeave:
            d->timeLine->stop();
            d->frame = 0;
            repaint();
            return false;
        default:
            return false;
    };
}

void RaptorScrollButton::animatePaint(int frame)
{
    d->frame = frame;
    repaint();
}
