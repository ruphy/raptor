/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumbitem.h"

RaptorBreadCrumbItem::RaptorBreadCrumbItem(QWidget * parent)
  : QPushButton(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
}

RaptorBreadCrumbItem::~RaptorBreadCrumbItem()
{
}

void RaptorBreadCrumbItem::paintEvent(QPaintEvent * event)
{
    QPushButton::paintEvent(event);
}
