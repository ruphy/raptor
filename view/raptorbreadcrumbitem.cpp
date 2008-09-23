/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumbitem.h"

#include <QPainter>

RaptorBreadCrumbItem::RaptorBreadCrumbItem(const QIcon & icon, const QString & text, QWidget * parent)
  : QPushButton(icon, text, parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMaximumSize(QSize(22, 22));
}

RaptorBreadCrumbItem::~RaptorBreadCrumbItem()
{
}

void RaptorBreadCrumbItem::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.drawPixmap(contentsRect(), icon().pixmap(22, 22));
}
