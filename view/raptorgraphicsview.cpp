/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorgraphicsview.h"
#include "raptormenuitem.h"
#include "raptoritemdelegate.h"

#include <QAbstractItemModel>
#include <QModelIndex>

class RaptorGraphicsView::Private
{
public:
    Private(RaptorGraphicsView *q) : q(q), model(0)
    {
        delegate = new RaptorItemDelegate(q);
    }

    RaptorGraphicsView *q;
    QAbstractItemModel *model;
    QModelIndex rootIndex;
    QSize itemsSize;
    QList<RaptorMenuItem*> items;
    QList<RaptorMenuItem*> shownItems;
    RaptorItemDelegate * delegate;
};

RaptorGraphicsView::RaptorGraphicsView(QGraphicsItem *parent) : QGraphicsWidget(parent), d(new Private(this))
{}

RaptorGraphicsView::~RaptorGraphicsView()
{
    delete d;
}

QModelIndex RaptorGraphicsView::rootIndex() const
{
    return d->rootIndex;
}

void RaptorGraphicsView::setRootIndex(const QModelIndex &index)
{
    d->rootIndex = index;

    // update();
}

QSize RaptorGraphicsView::itemsSize() const
{
    return d->itemsSize;
}

void RaptorGraphicsView::setItemsSize(const QSize &size)
{
    d->itemsSize = size;
    // update();
}

QAbstractItemModel* RaptorGraphicsView::model()
{
    return d->model;
}

void RaptorGraphicsView::setModel(QAbstractItemModel *model)
{
    d->model = model;
    // update();
}

void RaptorGraphicsView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
}

void RaptorGraphicsView::getItems()
{
    for (int i = 0; i < d->model->rowCount(d->rootIndex); i++) {
        d->items << new RaptorMenuItem(d->model->index(i, 0, d->rootIndex), d->delegate, this);
    }
}

void RaptorGraphicsView::retrieveShownItems()
{
    d->shownItems.clear();
    int n;
    if ((float)(size().width() / itemsSize().width())%10 != 0) { // if we don't have an x.y type number
        n = size().width() / itemsSize().width();
    }
    else { //Probably we show a half-item?
        n = (size().width() / itemsSize().width()) + 1;
    }
    if (n >= d->items.count()) {
        d->shownItems = d->items;
    } else {
        for (int i = 0; i < n; i++) {
            d->shownItems << d->items.at(i);
        }
    }
}
