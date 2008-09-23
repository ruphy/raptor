/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORBREADCRUMBITEM_H
#define RAPTORBREADCRUMBITEM_H

#include <QPushButton>

class QPaintEvent;

class RaptorBreadCrumbItem : public QPushButton
{
    Q_OBJECT
    public:
        RaptorBreadCrumbItem(const QIcon & icon, const QString & text, QWidget * parent = 0);
        ~RaptorBreadCrumbItem();

        void paintEvent(QPaintEvent * event);
};

#endif
