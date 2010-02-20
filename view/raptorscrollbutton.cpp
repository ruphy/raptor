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
#include <QTimeLine>
#include <QHoverEvent>
#include <QPaintEvent>

#include <KDebug>

const int FRAMES = 5;

class RaptorScrollButton::Private
{
    public:
        Private(Side s, RaptorScrollButton * button)
          : q(button),
            side(s),
            frame(0)
        {
            svg = new Plasma::Svg(q);
            svg->setImagePath("widgets/raptorarrows");
            svg->setContainsMultipleImages(true);
            timeLine = new QTimeLine(150, button);
            timeLine->setFrameRange(0, FRAMES);
        }
        ~Private()
        {}

        RaptorScrollButton * q;
        Side side;
        Plasma::Svg * svg;
        QTimeLine * timeLine;
        int frame;
        QRectF rect;
};
RaptorScrollButton::RaptorScrollButton(Side side, QWidget * parent)
  : QWidget(parent),
    d(new Private(side, this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    update();
    installEventFilter(this);
    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(animatePaint(int)));
}

RaptorScrollButton::~RaptorScrollButton()
{
}

void RaptorScrollButton::paintEvent(QPaintEvent * event)
{
    qreal buttonSize = size().width();

    QPainter p(this);
    QRectF r(event->rect());
    
//     qreal buttonRatioWidth = 0.7;
//     qreal buttonRatioHeight = 0.3;

    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);

//     r.setSize(QSizeF(contentsRect().size().width() * buttonRatioWidth + d->frame, 
//                      contentsRect().size().height() * buttonRatioHeight + d->frame));

    r.setSize(QSizeF(buttonSize - FRAMES + d->frame, buttonSize - FRAMES + d->frame));

    d->svg->resize(r.size());
    r.moveCenter(contentsRect().center());
    d->rect = r;

    if (d->side == Right) {
       d->svg->paint(&p, r, "rightarrow");
    }
    else {
       d->svg->paint(&p, r, "leftarrow");
    }
}

bool RaptorScrollButton::eventFilter(QObject * watched, QEvent * event)
{
    Q_UNUSED(watched)
    switch(event->type())
    {
        case QEvent::HoverEnter:
        case QEvent::HoverMove:
            if (d->frame != FRAMES && d->rect.contains(static_cast<QHoverEvent*>(event)->pos())) {
                d->timeLine->stop();
                d->timeLine->setDirection(QTimeLine::Forward);
                d->timeLine->start();
                break;
            }
        case QEvent::HoverLeave:
            if (d->frame == FRAMES && !d->rect.contains(static_cast<QHoverEvent*>(event)->pos())) {
                d->timeLine->stop();
                d->timeLine->setDirection(QTimeLine::Backward);
                d->timeLine->start();
                update();
            }
            break;
        case QEvent::MouseButtonPress:
            if (d->frame) {
                emit clicked();
            }
            break;
        default:
            break;
    };
    return false;
}

void RaptorScrollButton::animatePaint(int frame)
{
    d->frame = frame;
    repaint();
}
