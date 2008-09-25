/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumb.h"
#include "raptorbreadcrumbitem.h"

#include "raptoritemsview.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QAbstractItemModel>

#include <KIcon>
#include <KDebug>

class RaptorBreadCrumb::Private
{
    public:
        Private(RaptorItemsView * v, QAbstractItemModel * m)
          : layout(0),
            view(v),
            model(m)
        {}
        ~Private() {}

        QGraphicsLinearLayout * layout;
        RaptorItemsView * view;
        QAbstractItemModel * model;
        QMap<int,RaptorBreadCrumbItem*> items;
};

RaptorBreadCrumb::RaptorBreadCrumb(RaptorItemsView * view, QAbstractItemModel * model, QGraphicsWidget * parent)
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
    QModelIndex index = d->view->currentIndex();
    if (d->model->parent(index).isValid())
        addCrumb(d->model->parent(index));
}

void RaptorBreadCrumb::addCrumb(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    for (int i = d->layout->count() - 1; i >= 0; i--) {
        d->layout->removeAt(i);
    }
    foreach (RaptorBreadCrumbItem *itm, d->items.values()) {
        itm->deleteLater();
    }
    d->items.clear();

    QModelIndex tmp = index;
    int position = 0;

    do {

        kDebug() << d->model->data(tmp).toString();
        RaptorBreadCrumbItem * item = new RaptorBreadCrumbItem(QIcon(d->model->data(tmp, Qt::DecorationRole)
                                                               .value<QIcon>()), d->model->data(tmp).toString(),
                                                               tmp);
        QGraphicsProxyWidget * proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(item);
        d->items[position] = item;
        d->layout->insertItem(0, proxy);
        d->layout->invalidate();

        connect(item, SIGNAL(navigationRequested(const QModelIndex &, RaptorBreadCrumbItem *)),
                this, SLOT(navigate(const QModelIndex &, RaptorBreadCrumbItem *)));

        tmp = d->model->parent(tmp);
        ++position;

    } while (tmp.isValid());

}

void RaptorBreadCrumb::navigate(const QModelIndex &index, RaptorBreadCrumbItem *item)
{
    kDebug() << "Navigation";
    if (index.isValid()) {
        d->view->setRootIndex(index.parent());
    } else {
        d->view->setRootIndex(index);
    }

    int key;

    kDebug() << "Now deleting items";

    for (key = d->items.key(item); key>=0; --key)
    {
        RaptorBreadCrumbItem *tmp = d->items[key];
        d->items.remove(key);
        tmp->deleteLater();
    }
}

#include "raptorbreadcrumb.moc"
