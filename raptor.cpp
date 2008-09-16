 /* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptor.h"
#include "view/raptoritemsview.h"
#include "view/raptoritemdelegate.h"
#include "engine/kickoff/applicationmodel.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QPalette>
 
#include <plasma/svg.h>
#include <plasma/theme.h>
 
Raptor::Raptor(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_svg(this),
    m_icon("plasma"),
    m_view(0)
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(Plasma::Applet::StandardBackground);
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
    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));
    setIcon("plasma");
    
} 


void Raptor::setupView()
{
    //WARNING: ruphy this is just for test, don't be scared :) (alediaferia)

    m_view = new RaptorItemsView();
    RaptorItemDelegate *delegate = new RaptorItemDelegate();
    //TODO: connect this to theme change
    delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));

    Kickoff::ApplicationModel *model = new Kickoff::ApplicationModel();

    // let's make the view nicer in the applet
    //m_view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    m_view->setAttribute(Qt::WA_NoSystemBackground);
    m_view->viewport()->setAutoFillBackground(true);
    QPalette p = m_view->viewport()->palette();
    p.setColor(QPalette::Base, Qt::transparent);
    m_view->viewport()->setPalette(p);

    m_view->setModel(model);
    m_view->setItemDelegate(delegate);

    m_view->hideScrollBars();
}

QWidget* Raptor::widget()
{
    return static_cast<QWidget*>(m_view);
}

void Raptor::updateColors()
{
    static_cast<RaptorItemDelegate*>(m_view->itemDelegate())->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
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
