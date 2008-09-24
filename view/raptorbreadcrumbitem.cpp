/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumbitem.h"

#include <QPainter>
#include <QModelIndex>

class RaptorBreadCrumbItem::Private
{
public:
    Private(RaptorBreadCrumbItem *q) : q(q)
    {}

    RaptorBreadCrumbItem *q;
    QModelIndex index;
};

RaptorBreadCrumbItem::RaptorBreadCrumbItem(const QIcon & icon, const QString & text, QWidget * parent)
  : QPushButton(icon, text, parent),
    d(new Private(this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMaximumSize(QSize(22, 22));

    connect(this, SIGNAL(clicked()), SLOT(emitNavigationRequested()));
}

RaptorBreadCrumbItem::~RaptorBreadCrumbItem()
{
    delete d;
}

void RaptorBreadCrumbItem::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.drawPixmap(contentsRect(), icon().pixmap(22, 22));
}

const QModelIndex RaptorBreadCrumbItem::index()
{
    return d->index;
}

void RaptorBreadCrumbItem::emitNavigationRequested()
{
    emit navigationRequested(index());
}
