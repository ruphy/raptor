/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumb.h"
#include "raptorbreadcrumbitem.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>

class RaptorBreadCrumb::Private
{
    public:
        Private() {}
        ~Private() {}

        QGraphicsLinearLayout * layout;
};

RaptorBreadCrumb::RaptorBreadCrumb(QGraphicsWidget * parent)
  : QGraphicsWidget(parent),
    d(new Private())
{
    d->layout = new QGraphicsLinearLayout(this);
    QGraphicsProxyWidget * proxy = new QGraphicsProxyWidget(this);
    RaptorBreadCrumbItem * item = new RaptorBreadCrumbItem();
    proxy->setWidget(item);
    d->layout->addItem(proxy);
}

RaptorBreadCrumb::~RaptorBreadCrumb()
{
}
