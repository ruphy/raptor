/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>,
   					Dariusz Mikulski <dariusz.mikulski@gmail.com>

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

namespace
{
	const QString BUTTON_ARROWS			= "widgets/raptorarrows";
	const QString LEFT_ARROW			= "leftarrow";
	const QString RIGHT_ARROW			= "rightarrow";
	const float BTN_WIDTH_DELTA			= 0.7;
	const float BTN_HEIGHT_DELTA		= 0.6;
	const int START_FRAME				= 0;
	const int END_FRAME					= 20;
}

class RaptorScrollButton::Private
{
    public:
        Private(Side s, RaptorScrollButton * button)
          : q(button),
            side(s),
            frame(0)
        {
            svg = new Plasma::Svg(q);
            svg->setImagePath(BUTTON_ARROWS);
            timeLine = new QTimeLine(250, button);
        }
        ~Private()
        {}

        RaptorScrollButton * q;
        Side side;
        Plasma::Svg * svg;
        QTimeLine * timeLine;
        int frame;
};
RaptorScrollButton::RaptorScrollButton(Side side, QWidget * parent)
  : QPushButton(parent),
    d(new Private(side, this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    repaint();
    installEventFilter(this);
    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(animatePaint(int)));
}

RaptorScrollButton::~RaptorScrollButton()
{
}

void RaptorScrollButton::paintEvent(QPaintEvent * event)
{
	Q_UNUSED(event);
	
    QPainter p(this);
    QRectF r(contentsRect());

	p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);

    r.setSize(QSizeF(contentsRect().size().width() * BTN_WIDTH_DELTA + d->frame, 
					 contentsRect().size().height() * BTN_HEIGHT_DELTA + d->frame));
	d->svg->resize(r.size());
    r.moveCenter(QPointF(contentsRect().size().width() / 2, contentsRect().size().height() / 2));

	if (d->side == Right) {
       d->svg->paint(&p, r, RIGHT_ARROW);
    }
    else {
       d->svg->paint(&p, r, LEFT_ARROW);
    }
}

bool RaptorScrollButton::eventFilter(QObject * watched, QEvent * event)
{
    Q_UNUSED(watched)
    switch(event->type())
    {
        case QEvent::HoverEnter:
            d->timeLine->setFrameRange(START_FRAME, END_FRAME);
            d->timeLine->start();
            return false;
        case QEvent::HoverLeave:
            d->timeLine->stop();
            d->frame = START_FRAME;
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
