/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

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
        QList<RaptorBreadCrumbItem*> items;
};

RaptorBreadCrumb::RaptorBreadCrumb(RaptorItemsView * view, QAbstractItemModel * model, QGraphicsWidget * parent)
  : QGraphicsWidget(parent),
    d(new Private(view, model))
{
    d->layout = new QGraphicsLinearLayout(this);
    connect(d->view, SIGNAL(clicked(const QModelIndex &)), SLOT(reload()));
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
    kDebug() << d->model->data(index).toString();
    RaptorBreadCrumbItem * item = new RaptorBreadCrumbItem();
    item->setIcon(QIcon(d->model->data(index, Qt::DecorationRole).value<QPixmap>()));
    item->setText(d->model->data(index).toString());
    QGraphicsProxyWidget * proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(item);
    d->items.append(item);
    d->layout->addItem(proxy);

    if (d->model->parent(index).isValid())
    {
        addCrumb(d->model->parent(index));
    }
}
