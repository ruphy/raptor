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
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <KDebug>

#include <Plasma/Theme>

class RaptorGraphicsView::Private
{
public:
    Private(RaptorGraphicsView *q) : q(q), model(0), scrollOffset(0.0)
    {
        delegate = new RaptorItemDelegate(q);
        delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    }

    RaptorGraphicsView *q;
    QAbstractItemModel *model;
    QModelIndex rootIndex;
    QList<RaptorMenuItem*> items;
    QList<RaptorMenuItem*> shownItems;
    RaptorItemDelegate * delegate;
    float scrollOffset;
};

RaptorGraphicsView::RaptorGraphicsView(QGraphicsItem *parent) : QGraphicsWidget(parent), d(new Private(this))
{
    setViewMode(RaptorGraphicsView::Normal);
    setAcceptHoverEvents(true);
    installEventFilter(this);
}

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

    getItems();
    setupItems();
    update();
}

void RaptorGraphicsView::scrollRight()
{
    if (d->items == d->shownItems)
        return;

    RaptorMenuItem *item = d->items.first();
    d->items.removeAll(item);
    d->items.append(item);

    setupItems();
    update();
}

void RaptorGraphicsView::scrollLeft()
{
    if (d->items == d->shownItems)
        return;

    RaptorMenuItem *item = d->items.last();
    d->items.removeAll(item);
    d->items.prepend(item);

    setupItems();
    update();
}

QAbstractItemModel* RaptorGraphicsView::model()
{
    return d->model;
}

void RaptorGraphicsView::setModel(QAbstractItemModel *model)
{
    d->model = model;
    d->rootIndex = QModelIndex();

    getItems();
    setupItems();
    update();
}

void RaptorGraphicsView::setViewMode(ViewMode viewMode)
{
    d->delegate->setViewMode((RaptorItemDelegate::ViewMode)viewMode);

    setupItems();
    update();
}

RaptorGraphicsView::ViewMode RaptorGraphicsView::viewMode()
{
    return (RaptorGraphicsView::ViewMode)d->delegate->viewMode();
}

void RaptorGraphicsView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

//     painter->fillRect(rect(), Qt::blue);

//     painter->setPen(Qt::green);
    foreach (RaptorMenuItem *item, d->shownItems) {
//         painter->drawRect(item->rect());
        d->delegate->paint(painter, *item->option(), item->modelIndex());
    }
}

void RaptorGraphicsView::getItems()
{
    d->items.clear();
    for (int i = 0; i < d->model->rowCount(d->rootIndex); i++) {
        d->items << new RaptorMenuItem(d->model->index(i, 0, d->rootIndex), this);
    }
}

void RaptorGraphicsView::setupItems()
{
    // NOTE: for each view mode we should setup items individually
    // WARNING: we suppose a horizontal view
    // WARNING: we still don't take care of partial item showing
    // TODO: remove tabs

    d->shownItems.clear();
    ViewMode mode = viewMode();

    if (mode == RaptorGraphicsView::Normal) {
        qreal sizesSum = 0;
        qreal size = contentsRect().height();
        foreach (RaptorMenuItem *item, d->items) {
            item->setRect(QRectF(QPointF(sizesSum, 0), QSizeF(size, size)));
            sizesSum += size;
            if (sizesSum > contentsRect().width()) {
                break;
            }
            d->shownItems << item;
        }
    }

    else if (mode == RaptorGraphicsView::SingleApp) {
        RaptorMenuItem *item = d->items.first(); 
        item->setRect(QRectF(QPointF(0, 0), contentsRect().size()));
    }

    else if (mode == RaptorGraphicsView::TwoApps) {
	qreal x = 0;
	for (int i = 0; i < 2; i++) {
	    RaptorMenuItem *item = d->items[i];
	    item->setRect(QRectF(QPointF(x, 0), QSizeF(contentsRect().width() / 2.0, contentsRect().height())));
	    x += contentsRect().width();
	}
     }

     else if (mode == RaptorGraphicsView::Search) {
	  qreal sizesSum = 0;
	  int i = 0;
	  qreal y = 0;

	  for (; i < 2; i++) { // we place the first two items half sized and in column
	      RaptorMenuItem *item = d->items[i];
	      item->setRect(QRectF(QPointF(0, y), QSizeF(contentsRect().height() / 2, contentsRect().height() / 2)));
	      y += contentsRect().height() / 2;
	  }
	  sizesSum += contentsRect().height() / 2;

	  for (; i < d->items.count(); i++) { // now we take care of left items
	      if (sizesSum > contentsRect().width() - (contentsRect().height() / 2)) {
		  sizesSum = contentsRect().width() - (contentsRect().height() / 2);
		  break;
	      }

	      if ( d->items.count() - i == 2 ) {
		  break;
	      }

	      RaptorMenuItem *item = d->items[i]; 
	      item->setRect(QRectF(QPointF(sizesSum, 0), QSizeF(contentsRect().height(), contentsRect().height())));
	      d->shownItems << item;

	      sizesSum += contentsRect().height();
	  }

	  int max = i + 2;
	  y = 0;
	  for (; i < max; i++) { // here we handle the last two items
	      RaptorMenuItem *item = d->items[i];
	      item->setRect(QRectF(QPointF(sizesSum, y), QSizeF(contentsRect().height() / 2, contentsRect().height() / 2)));
	      y += contentsRect().height() / 2;
	  }
    }

}

void RaptorGraphicsView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_UNUSED(event)
    setupItems();
    update();

    kDebug() << contentsRect();
}

void RaptorGraphicsView::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    foreach (RaptorMenuItem *item, d->shownItems) {
        item->option()->state = QStyle::State_None;
        if (item->rect().contains(event->pos())) {
            item->option()->state = QStyle::State_MouseOver;
        }
    }
    update();
}

bool RaptorGraphicsView::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress) {
        QGraphicsSceneMouseEvent * e = static_cast<QGraphicsSceneMouseEvent*>(event);
        foreach (RaptorMenuItem * item, d->shownItems) {
            if (item->rect().contains(e->pos())) {
                setRootIndex(item->modelIndex());
                break;
            }
        }
    }
    return false;
}
