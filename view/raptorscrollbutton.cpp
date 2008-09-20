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

#include <KDebug>

class RaptorScrollButton::Private
{
    public:
        Private(Side s)
          : side(s)
        {}
        ~Private()
        {}

        Side side;
        Plasma::Svg * svg;
};
RaptorScrollButton::RaptorScrollButton(Side side, QWidget * parent)
  : QPushButton(parent),
    d(new Private(side))
{
    d->svg = new Plasma::Svg(this);
    d->svg->setImagePath("widgets/raptorarrows");
    setAttribute(Qt::WA_NoSystemBackground);
    repaint();
}

RaptorScrollButton::~RaptorScrollButton()
{
}

void RaptorScrollButton::paintEvent(QPaintEvent * event)
{
    kDebug();
    QPainter p(this);
    QRectF r(event->rect());
    r.setSize(size());
    kDebug() << r.size();
    r.setSize(QSizeF(r.width() * 0.7, r.height() * 0.6));
    kDebug() << r.size();
    QPointF point = r.center();
    kDebug() << point;
    point.setX(point.x() - r.width() / 4);
    point.setY(point.y() - r.height() / 4);
    kDebug() << point;
    r.moveTo(point);
    if (d->side == Right) {
       d->svg->paint(&p, r, "rightarrow");
    }
    else {
       d->svg->paint(&p, r, "leftarrow");
    }
}
