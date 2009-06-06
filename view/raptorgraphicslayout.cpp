/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorgraphicslayout.h"

#include "raptorgraphicsview.h"
#include "raptormenuitem.h"

#include <QEvent>
#include <QTimeLine>

#include <KDebug>

const int DURATION = 250;

class RaptorGraphicsLayout::Private
{
public:
    Private(RaptorGraphicsLayout *q) : q(q), view(0), temporaryVisibleItem(0), topMargin(0), leftMargin(0), rightMargin(0), bottomMargin(0)
    {
        scrollTimeLine = new QTimeLine(DURATION, q);
        scrollTimeLine->setFrameRange(0, 20);
    }
    ~Private()
    {}

    RaptorGraphicsLayout *q;
    RaptorGraphicsView *view;
    QList<RaptorMenuItem*> items;
    QList<RaptorMenuItem*> visibleItems;
    RaptorMenuItem *temporaryVisibleItem;
    QTimeLine * scrollTimeLine;

    qreal topMargin;
    qreal leftMargin;
    qreal rightMargin;
    qreal bottomMargin;

    void layoutItems();
};

RaptorGraphicsLayout::RaptorGraphicsLayout(RaptorGraphicsView *parent) : QObject(parent), d(new Private(this))
{
    d->view = parent;
    d->view->installEventFilter(this);
    connect (d->scrollTimeLine, SIGNAL(frameChanged(int)), SLOT(setScrollingFrame()));
}

RaptorGraphicsLayout::~RaptorGraphicsLayout()
{
    delete d;
}

void RaptorGraphicsLayout::setMenuItems(const QList<RaptorMenuItem*> &items)
{
    d->items.clear();
    d->items << items;
}

QList<RaptorMenuItem*> RaptorGraphicsLayout::visibleItems() const
{
    if (d->scrollTimeLine->state() == QTimeLine::NotRunning || !d->temporaryVisibleItem) {
        return d->visibleItems;
    } else {
        return QList<RaptorMenuItem*>() << d->visibleItems << d->temporaryVisibleItem;
    }
}

void RaptorGraphicsLayout::setContentMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    d->topMargin = top;
    d->leftMargin = left;
    d->rightMargin = right;
    d->bottomMargin = bottom;
}

bool RaptorGraphicsLayout::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != d->view) {
        return false;
    }

    if (event->type() == QEvent::GraphicsSceneResize) {
        d->layoutItems();
        d->view->update();
        return true;
    }

    return false;
}

void RaptorGraphicsLayout::invalidate()
{
    d->layoutItems();
}

void RaptorGraphicsLayout::setScrollingFrame()
{
    foreach (RaptorMenuItem * item, d->visibleItems) {
        item->setAnimationValue(d->scrollTimeLine->currentValue());
    }
    if (d->temporaryVisibleItem) {
        d->temporaryVisibleItem->setAnimationValue(d->scrollTimeLine->currentValue());
    }
    /*foreach (RaptorMenuItem * item, d->needsAnimation) {
        item->setAnimationValue(d->scrollTimeLine->currentValue());
    }*/
    d->view->update();
}

void RaptorGraphicsLayout::Private::layoutItems()
{
    if (items.isEmpty()) {
        visibleItems.clear();
        return;
    }
    // NOTE: for each view mode we should setup items individually
    // WARNING: we suppose a horizontal view
    // TODO: remove tabs
    QList<RaptorMenuItem*> oldVisibleItems = visibleItems;
    visibleItems.clear();
//     needsAnimation.clear();
    RaptorGraphicsView::ViewMode mode = view->viewMode();

    QRectF rect = view->rect();
    rect.setSize(QSizeF(rect.width() - (leftMargin + rightMargin), rect.height() - (topMargin + bottomMargin)));
    rect.translate(topMargin, leftMargin);

    if (mode == RaptorGraphicsView::Normal) {
        kDebug() << "YAY";
        qreal sizesSum = 0;
        const qreal size = rect.height();
        foreach (RaptorMenuItem *item, items) {
            if (sizesSum - size > rect.width()) {
                item->setRect(QRectF(QPointF(sizesSum +  size, topMargin), QSizeF(size, size)));
                item->moveTo(QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size)));
                sizesSum += size;
                break;
            }

            if (item == items.first() && !oldVisibleItems.contains(item)) {
                kDebug() << "SET FUCKIN NEGATIVE RECT" << item->modelIndex().data(Qt::DisplayRole) << oldVisibleItems.contains(item);
                item->setRect(QRectF(QPointF(-1 * size, topMargin), QSizeF(size, size)));
            }
            kDebug() << "Set item rect" << QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size));
            //item->setRect(QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size)));
            item->moveTo(QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size)));
            sizesSum += size;

            visibleItems << item;
        }
        if (!visibleItems.contains(items.last()) && !items.last()->rect().left()) {
            items.last()->moveTo(QRectF(QPointF(-1 * size, topMargin), QSizeF(size, size)));
            temporaryVisibleItem = items.last();
        }
    }

    else if (mode == RaptorGraphicsView::SingleApp) {
        kDebug() << "SINGLE APP";
        RaptorMenuItem *item = items.first();
        if (!oldVisibleItems.isEmpty()) {
            if (oldVisibleItems.first() == items.last()) {
                oldVisibleItems.first()->moveTo(QRectF(QPointF(-rect.width(), 0), rect.size()));
                item->setRect(QRectF(QPointF(rect.width(), 0), rect.size()));
                item->moveTo(QRectF(QPointF(0, 0), rect.size()));
            } else {
                oldVisibleItems.first()->moveTo(QRectF(QPointF(rect.width(), 0), rect.size()));
                item->setRect(QRectF(QPointF(-rect.width(), 0), rect.size()));
                item->moveTo(QRectF(QPointF(0, 0), rect.size()));
            }
            temporaryVisibleItem = oldVisibleItems.first();
        } else {
            item->setRect(QRectF(QPointF(0, 0), rect.size()));//Don't use rect for single-app, we don't need a margin here
        }
        visibleItems << item;
    }

    else if (mode == RaptorGraphicsView::TwoApps) {
        qreal x = 0;
        for (int i = 0; i < 2; i++) {
            if (items.count() <= i) {
                break;
            }
            RaptorMenuItem *item = items[i];
            if (oldVisibleItems.isEmpty()) {
                item->setRect(QRectF(QPointF(x, topMargin), QSizeF(rect.width() / 2, rect.height())));
            } else if (oldVisibleItems.first() == items.last()) {
                temporaryVisibleItem = oldVisibleItems.first();
                temporaryVisibleItem->moveTo(QRectF(QPointF(-rect.width() / 2, topMargin), QSizeF(rect.width() / 2, rect.height())));
                item->setRect(QRectF(QPointF(x + rect.width() / 2, topMargin), QSizeF(rect.width() / 2, rect.height())));
                item->moveTo(QRectF(QPointF(x, topMargin), QSizeF(rect.width() / 2, rect.height())));
            } else {
                temporaryVisibleItem = oldVisibleItems.last();
                temporaryVisibleItem->moveTo(QRectF(QPointF(rect.width() + rect.width() / 2, topMargin), QSizeF(rect.width() / 2, rect.height())));
                item->setRect(QRectF(QPointF(x - rect.width() / 2, topMargin), QSizeF(rect.width() / 2, rect.height())));
                item->moveTo(QRectF(QPointF(x, topMargin), QSizeF(rect.width() / 2, rect.height())));
            }
            x += rect.width() / 2;
            visibleItems << item;
        }
     }

    else if (mode == RaptorGraphicsView::Search) {
        qreal sizesSum = 0;
        int i = 0;
        qreal y = topMargin;

        for (; i < 2; i++) { // we place the first two items half sized and in column
            RaptorMenuItem *item = items[i];
            if (oldVisibleItems.isEmpty()) {
                item->setRect(QRectF(QPointF(0, y), QSizeF(rect.height(), rect.height() / 2)));
            } else {
                item->moveTo(QRectF(QPointF(0, y), QSizeF(rect.height(), rect.height() / 2)));
            }
            y += rect.height() / 2;
            visibleItems << item;
        }
        sizesSum += rect.height();

        for (; i < items.count(); i++) { // now we take care of left items
            if (sizesSum > rect.width() - (rect.height())) {
                 sizesSum = rect.width() - (rect.height());
                 break;
            }

            if ( items.count() - i == 2 ) {
                break;
            }

            RaptorMenuItem *item = items[i];
            if (oldVisibleItems.isEmpty()) {
                item->setRect(QRectF(QPointF(sizesSum, topMargin), QSizeF(rect.height(), rect.height())));
            } else {
                item->moveTo(QRectF(QPointF(sizesSum, topMargin), QSizeF(rect.height(), rect.height())));
            }
            visibleItems << item;

            sizesSum += rect.height();
        }

        int max = i + 2;
        y = topMargin;
        for (; i < max; i++) { // here we handle the last two items
            RaptorMenuItem *item = items[i];
            if (oldVisibleItems.isEmpty()) {
                item->setRect(QRectF(QPointF(sizesSum, y), QSizeF(rect.height(), rect.height() / 2)));
            } else {
                item->moveTo(QRectF(QPointF(sizesSum, y), QSizeF(rect.height(), rect.height() / 2)));
            }
            y += rect.height() / 2;
            visibleItems << item;
        }
    }
    
    if (scrollTimeLine->state() == QTimeLine::Running) {
        scrollTimeLine->stop();
    }
    scrollTimeLine->start();

}
