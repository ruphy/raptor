 /* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptor.h"
#include "view/raptorgraphicswidget.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>

#include <plasma/svg.h>
#include <plasma/theme.h>

Raptor::Raptor(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_svg(this),
    m_icon("start-here-kde"),
    m_gwidget(0)
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(Plasma::Applet::StandardBackground);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    resize(200, 200);
}


Raptor::~Raptor()
{
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}

void Raptor::init()
{
    // A small demonstration of the setFailedToLaunch function
    if (m_icon.isNull()) {
        setFailedToLaunch(true, "No world to say hello");
    }

    setupView();

    setPopupIcon("start-here");
}


void Raptor::setupView()
{
    m_gwidget = new RaptorGraphicsWidget(this, globalConfig());
}

QGraphicsWidget* Raptor::graphicsWidget()
{
    return m_gwidget;
}

void Raptor::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
//     p->setRenderHint(QPainter::SmoothPixmapTransform);
//     p->setRenderHint(QPainter::Antialiasing);
//
//     // Now we draw the applet, starting with our svg
//     m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
//     m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());
//
//     // We place the icon and text
//     p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(),(int)contentsRect.width()-14));
//     p->save();
//     p->setPen(Qt::white);
//     p->drawText(contentsRect,
//                 Qt::AlignBottom | Qt::AlignHCenter,
//                 "Hello Plasmoid!");
//     p->restore();

    PopupApplet::paintInterface(p, option, contentsRect);
}

#include "raptor.moc"
