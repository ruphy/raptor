 /* This file is part of the KDE project

   Copyright (C) 2008 - 2009 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptor.h"
#include "view/raptorgraphicswidget.h"
#include "raptordialog.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QGraphicsLinearLayout>

#include <KDebug>

#include <plasma/theme.h>
#include <Plasma/Corona>

Raptor::Raptor(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_icon(0),
      m_dialog(0),
      m_gwidget(0)
{
    //setAspectRatioMode(Plasma::IgnoreAspectRatio);
    //resize(200, 200);
}

Raptor::~Raptor()
{
    delete m_dialog;
}

void Raptor::constraintsEvent(Plasma::Constraints constraints)
{
    Q_UNUSED(constraints)
    setBackgroundHints(NoBackground);
}

void Raptor::init()
{
    QGraphicsLinearLayout * layout = new QGraphicsLinearLayout(this);
    m_icon = new Plasma::IconWidget(KIcon("start-here"), QString(), this);
    layout->addItem(m_icon);
    setLayout(layout);

    connect (m_icon, SIGNAL(clicked()), SLOT(popup()));
}

void Raptor::popup()
{
    if (!m_dialog) {
        qobject_cast<Plasma::Corona*>(graphicsWidget()->scene())->addOffscreenWidget(graphicsWidget());
        m_dialog = new RaptorDialog();
        m_dialog->setResizeHandleCorners(Plasma::Dialog::All);
        m_dialog->setGraphicsWidget(graphicsWidget());
    }
    if (m_dialog->isVisible()) {
        m_dialog->hide();
    } else {
        updateDialog();
        m_dialog->show();
    }
}

QGraphicsWidget* Raptor::graphicsWidget()
{
    if (!m_gwidget) {
        m_gwidget = new RaptorGraphicsWidget(this, globalConfig());
        m_gwidget->setMinimumSize(64, 32);
        m_gwidget->setPreferredSize(300, 128);
    }
    return m_gwidget;
}

void Raptor::updateDialog()
{
//     kDebug() << m_gwidget->preferredSize().toSize();
    m_dialog->resize(m_gwidget->preferredSize().toSize()); // WARNING: this does not work!!
    m_dialog->move(qobject_cast<Plasma::Corona*>(graphicsWidget()->scene())->popupPosition(graphicsWidget(), m_dialog->size())); // WARNING: pretty random
//     kDebug() << m_dialog->size();
}

#include "raptor.moc"
