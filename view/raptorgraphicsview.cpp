/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

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
#include <QList>

class RaptorGraphicsView::Private
{
public:
    Private(RaptorGraphicsView *q) : q(q), model(0), scrollOffset(0.0)
    {
        delegate = new RaptorItemDelegate(q);
    }

    RaptorGraphicsView *q;
    QAbstractItemModel *model;
    QModelIndex rootIndex;
    QList<RaptorMenuItem*> items;
    QList<RaptorMenuItem*> shownItems;
    RaptorItemDelegate * delegate;
    RaptorGraphicsView::ViewMode viewMode;
    float scrollOffset;
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

void RaptorGraphicsView::scrollRight()
{
    if (d->items == d->shownItems)
        return;
    d->shownItems.removeAll(d->shownItems.first());
    if (d->shownItems.last() == d->items.last()) { //we're at the end of the items so let's begin from the beginning
        d->shownItems.append(d->items.first());
    }
    else {
        d->shownItems.append(d->items.at(d->items.indexOf(d->shownItems.last()) + 1));
    }
}

void RaptorGraphicsView::scrollLeft()
{
    if (d->items == d->shownItems)
        return;
    d->shownItems.removeAll(d->shownItems.last());
    if (d->shownItems.first() == d->items.first()) { //we're at the beginning of the items so let's begin from the beginning
        d->shownItems.append(d->items.last());
    }
    else {
        d->shownItems.append(d->items.at(d->items.indexOf(d->shownItems.first()) - 1));
    }
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

void RaptorGraphicsView::setViewMode(ViewMode viewMode)
{
    d->viewMode = viewMode;
    // update();
}

RaptorGraphicsView::ViewMode RaptorGraphicsView::viewMode()
{
    return d->viewMode;
}

void RaptorGraphicsView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    switch (d->viewMode) {
        case SingleApp:
        break;
        case TwoApps:
        break;
        case Normal:
        break;
        case Search:
        break;
    }
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
    // TODO: we should think about this code better:
    //  - Take care of the current view mode while calculating shown items
    //

//     int n;
//     if ((float)(size().width() / itemsSize().width())%10 != 0) { // if we don't have an x.y type number
//         n = size().width() / itemsSize().width();
//     }
//     else { //Probably we show a half-item?
//         n = (size().width() / itemsSize().width()) + 1;
//     }
//     if (n >= d->items.count()) {
//         d->shownItems = d->items;
//     } else {
//         for (int i = 0; i < n; i++) {
//             d->shownItems << d->items.at(i);
//         }
//     }
}

QSizeF RaptorGraphicsView::calculateItemSize(const QModelIndex &index)
{
    QSizeF size;

    switch (viewMode()) {
        case RaptorGraphicsView::Normal :
            size = QSizeF(contentsRect().height(), contentsRect().height());
            break;
        case RaptorGraphicsView::SingleApp :
            size = contentsRect().size();
            break;
        case RaptorGraphicsView::TwoApps :
            size = QSizeF(contentsRect().width() / 2.0, contentsRect().height());
            break;
        case RaptorGraphicsView::Search :
            retrieveShownItems();
            bool small = false;
            int i = 0;
            for (; i != d->shownItems.count(); i++) {
                if (index == d->shownItems.at(i)->modelIndex()) { //Get the the position of the index in a list of shownItems
                    break;
                }
            }

            if (i < 2 || d->shownItems.count() - 1 == i || d->shownItems.count() - 2 == i) { //Check whether it's a small one or not (last 2 and first 2 items)
                small = true;
            }

            if (small) { // the case for half big items..
                size = QSizeF(contentsRect().height() / 2, contentsRect().height() / 2);
            } else {
                size = QSizeF(contentsRect().height(), contentsRect().height());
            }
            break;
    }

    return size;
}