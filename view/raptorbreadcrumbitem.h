/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORBREADCRUMBITEM_H
#define RAPTORBREADCRUMBITEM_H

#include <QPushButton>

class QPaintEvent;
class QModelIndex;
class QAbstractItemModel;

class RaptorBreadCrumbItem : public QPushButton
{
    Q_OBJECT
    public:
        RaptorBreadCrumbItem(const QIcon & icon, const QString & text,
                             const QModelIndex &index, QWidget * parent = 0);
        ~RaptorBreadCrumbItem();

        void paintEvent(QPaintEvent * event);

        const QModelIndex index();

    signals:
        void navigationRequested(const QModelIndex &, RaptorBreadCrumbItem *);

    private slots:
        void emitNavigationRequested();

    private:
        class Private;
        Private *d;
};

class RaptorBreadCrumbArrow : public RaptorBreadCrumbItem
{
    Q_OBJECT
    public:
        RaptorBreadCrumbArrow(const QModelIndex &index, QAbstractItemModel * model, QWidget * parent = 0);
        ~RaptorBreadCrumbArrow();

    private:
        class Private;
        Private * d;
};

#endif
