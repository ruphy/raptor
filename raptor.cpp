 /* This file is part of the KDE project

   Copyright (C) 2008 - 2010 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptor.h"
#include "view/raptorgraphicswidget.h"
#include "view/raptorscrollbutton.h"
#include "raptordialog.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QGraphicsLinearLayout>

#include <KDebug>

#include <plasma/theme.h>
#include <Plasma/Corona>

Raptor::Raptor(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
      m_gwidget(0),
      m_rightScrollButton(0),
      m_leftScrollButton(0)
{
}

Raptor::~Raptor()
{
}

void Raptor::constraintsEvent(Plasma::Constraints constraints)
{
    Q_UNUSED(constraints)
    
    updateScrollButtonPositions();
}

void Raptor::init()
{
    setPopupIcon("kde-start-here");

    m_gwidget = new RaptorGraphicsWidget(this, globalConfig());
    m_gwidget->setMinimumSize(300, 128);
    m_gwidget->setPreferredSize(300, 128);
    m_leftScrollButton = new RaptorScrollButton(RaptorScrollButton::Left, 0);
    m_rightScrollButton = new RaptorScrollButton(RaptorScrollButton::Right, 0);
    m_leftScrollButton->resize(32, 32);
    m_rightScrollButton->resize(32, 32);
    m_leftScrollButton->show();
    m_rightScrollButton->show();
    updateScrollButtonPositions();   
    connect(m_leftScrollButton, SIGNAL(clicked()), m_gwidget, SLOT(scrollLeft()));
    connect(m_rightScrollButton, SIGNAL(clicked()), m_gwidget, SLOT(scrollRight()));
}

QGraphicsWidget* Raptor::graphicsWidget()
{
    return m_gwidget;
}

void Raptor::updateScrollButtonPositions()
{
    m_leftScrollButton->move(view()->mapToGlobal(mapToParent(m_gwidget->pos()).toPoint()).x() - m_leftScrollButton->size().width() / 2, 
                             view()->mapToGlobal(mapToParent(m_gwidget->pos()).toPoint()).y() + m_gwidget->size().height() / 2 - m_leftScrollButton->size().height() / 2);
    m_rightScrollButton->move(view()->mapToGlobal(mapToParent(m_gwidget->pos()).toPoint()).x() + m_gwidget->size().width() - m_rightScrollButton->size().width() / 2, 
                             view()->mapToGlobal(mapToParent(m_gwidget->pos()).toPoint()).y() + m_gwidget->size().height() / 2 - m_rightScrollButton->size().height() / 2);
    
}

void Raptor::popupEvent(bool shown)
{
    kDebug() << m_gwidget->size();
    if (shown) {
        m_leftScrollButton->show();
        m_rightScrollButton->show();
        updateScrollButtonPositions();
    }
}

#include "raptor.moc"
