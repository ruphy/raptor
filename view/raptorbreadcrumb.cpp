/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>
   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumb.h"
#include "raptorbreadcrumbitem.h"

#include "raptorgraphicsview.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QAbstractItemModel>

#include <KIcon>
#include <KDebug>
#include <klocalizedstring.h>

class RaptorBreadCrumb::Private
{
    public:
        Private(RaptorGraphicsView * v, QAbstractItemModel * m)
          : layout(0),
            view(v),
            model(m)
        {}
        ~Private() {}

        QGraphicsLinearLayout * layout;
        RaptorGraphicsView * view;
        QAbstractItemModel * model;
        QMap<int,RaptorBreadCrumbItem*> items;
        QColor textColor;
};

RaptorBreadCrumb::RaptorBreadCrumb(RaptorGraphicsView * view, QAbstractItemModel * model, QGraphicsWidget * parent)
  : QGraphicsWidget(parent),
    d(new Private(view, model))
{
    d->layout = new QGraphicsLinearLayout(this);
    connect(d->view, SIGNAL(enteredItem(const QModelIndex &)), this, SLOT(addCrumb(const QModelIndex &)));
    reload();
}

RaptorBreadCrumb::~RaptorBreadCrumb()
{
    qDeleteAll(d->items);
}

void RaptorBreadCrumb::reload()
{
    kDebug();
    //qDeleteAll(d->items);
    //for (int i = 0; i != d->layout->count(); i++)
    //    d->layout->removeAt(i);
    QModelIndex index = d->view->rootIndex();
    if (d->model->parent(index).isValid()) {
        addCrumb(d->model->parent(index));
    } else {
        addCrumb(QModelIndex());
    }
}

void RaptorBreadCrumb::addCrumb(const QModelIndex & index)
{
    kDebug() << "Ouh yeah";
    for (int i = d->layout->count() - 1; i >= 0; i--) {
        d->layout->removeAt(i);
    }

    foreach (RaptorBreadCrumbItem *itm, d->items.values()) {
        itm->deleteLater();
    }
    d->items.clear();


    if (!index.isValid()) {
        RaptorBreadCrumbItem * item = new RaptorBreadCrumbItem(KIcon("go-home"), i18n("Main Menu"),
                QModelIndex());
        d->items[0] = item;
        d->layout->insertItem(0, item);
        d->layout->invalidate();

        connect(item, SIGNAL(navigationRequested(const QModelIndex &, RaptorBreadCrumbItem *)),
                this, SLOT(navigate(const QModelIndex &, RaptorBreadCrumbItem *)));

        return;
    }

    QModelIndex tmp = index;
    int position = 0;

    do {

        kDebug() << d->model->data(tmp).toString();
        RaptorBreadCrumbItem * item = new RaptorBreadCrumbItem(QIcon(d->model->data(tmp, Qt::DecorationRole)
                                                               .value<QIcon>()), d->model->data(tmp).toString(),
                                                               tmp);
        d->items[position] = item;
        d->layout->insertItem(0, item);

        connect(item, SIGNAL(navigationRequested(const QModelIndex &, RaptorBreadCrumbItem *)),
                this, SLOT(navigate(const QModelIndex &, RaptorBreadCrumbItem *)));

        ++position;

        kDebug() << d->model->data(tmp).toString();
        RaptorBreadCrumbItem * arrow = new RaptorBreadCrumbArrow(tmp, d->model);
        d->items[position] = arrow;
        d->layout->insertItem(0, arrow);
        d->layout->invalidate();

        connect(arrow, SIGNAL(navigationRequested(const QModelIndex &, RaptorBreadCrumbItem *)),
                this, SLOT(navigate(const QModelIndex &, RaptorBreadCrumbItem *)));

        ++position;
        tmp = d->model->parent(tmp);

    } while (tmp.isValid());

    RaptorBreadCrumbItem * item = new RaptorBreadCrumbItem(KIcon("go-home"), i18n("Main Menu"),
                                                           QModelIndex());
    d->items[position] = item;
    d->layout->insertItem(0, item);
    d->layout->invalidate();

    connect(item, SIGNAL(navigationRequested(const QModelIndex &, RaptorBreadCrumbItem *)),
            this, SLOT(navigate(const QModelIndex &, RaptorBreadCrumbItem *)));

}

void RaptorBreadCrumb::navigate(const QModelIndex &index, RaptorBreadCrumbItem *item)
{

    if (index == d->view->rootIndex()) {
        return;
    }

    kDebug() << "Navigation";

    for (int i = d->layout->count() - 1; i >= 0; i--) {
        d->layout->removeAt(i);
    }

    d->layout->invalidate();

    d->view->setRootIndex(index);

    int key;

    kDebug() << "Now deleting items";

    for (key = d->items.key(item) - 1; key>=0; --key) {
        RaptorBreadCrumbItem *tmp = d->items[key];
        d->items.remove(key);
        tmp->deleteLater();
    }

    if (d->items.count() == 1) {
        emit bottomLevelReached();
    }

    kDebug() << "Items successfully deleted";
}

#include "raptorbreadcrumb.moc"
