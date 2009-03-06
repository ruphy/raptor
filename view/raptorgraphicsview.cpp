/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorgraphicsview.h"
#include "raptoritemdelegate.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QList>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTimeLine>

#include <KDebug>

#include <Plasma/Theme>

class RaptorGraphicsView::Private
{
public:
    Private(RaptorGraphicsView *q) : q(q), model(0), currentHoveredItem(0), scrollOffset(0)
    {
        delegate = new RaptorItemDelegate(q);
        delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    }

    ~Private()
    { qDeleteAll(items); items.clear(); shownItems.clear(); }

    RaptorGraphicsView *q;

    QAbstractItemModel *model;
    QModelIndex rootIndex;

    QList<RaptorMenuItem*> items;
    QList<RaptorMenuItem*> shownItems;

    RaptorItemDelegate * delegate;
    
    RaptorMenuItem *currentHoveredItem;

    qreal scrollOffset;
};

RaptorGraphicsView::RaptorGraphicsView(QGraphicsItem *parent) : QGraphicsWidget(parent), d(new Private(this))
{
    setViewMode(RaptorGraphicsView::Normal);
    setAcceptHoverEvents(true);

    connect (d->delegate, SIGNAL(favoriteAddRequested(const QModelIndex &)), this, SLOT(slotAddFavorite(const QModelIndex &)));
}

RaptorGraphicsView::~RaptorGraphicsView()
{
    delete d;
}

QModelIndex RaptorGraphicsView::rootIndex() const
{
    return d->rootIndex;
}

QList<RaptorMenuItem*> RaptorGraphicsView::items() const
{
    return d->items;
}

QList<RaptorMenuItem*> RaptorGraphicsView::shownItems() const
{
    return d->shownItems;
}

void RaptorGraphicsView::setRootIndex(const QModelIndex &index)
{
    if (index == d->rootIndex) {
        return;
    }
    if (!index.data(Qt::UserRole + 2).isNull()) {
        emit applicationClicked(KUrl(index.data(Qt::UserRole + 2).toString())); //Qt::UserRole + 2 is Kickoff::UrlRole
        return;
    }
    d->rootIndex = index;

    getItems();
    setupItems();
    update();
    emit enteredItem(d->rootIndex);
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

    painter->setClipRect(contentsRect());
//     painter->fillRect(rect(), Qt::red);

    foreach (RaptorMenuItem *item, d->shownItems) {
        d->delegate->paint(painter, *item->option(), item->modelIndex());
    }
}

void RaptorGraphicsView::getItems()
{
//     kDebug() << "Get items!" << d->model->rowCount(d->rootIndex);
    qDeleteAll(d->items);
    d->currentHoveredItem = 0;
    d->items.clear();
    if (d->model->canFetchMore(d->rootIndex)) {
        d->model->fetchMore(d->rootIndex);
    }
    for (int i = 0; i < d->model->rowCount(d->rootIndex); i++) {
        kDebug() << "Get Item" << i << "Name:" << d->model->index(i, 0, d->rootIndex).data(Qt::DisplayRole);
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
        qreal sizesSum = d->scrollOffset;
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

void RaptorGraphicsView::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    foreach (RaptorMenuItem *item, d->shownItems) {
        item->option()->state = QStyle::State_None;
        if (item->rect().contains(event->pos())) {
            item->option()->state = QStyle::State_MouseOver;
            item->timeLine()->setDirection(QTimeLine::Forward);
            item->timeLine()->start();
            d->currentHoveredItem = item;
        }
    }
    update();
}

void RaptorGraphicsView::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    foreach (RaptorMenuItem *item, d->shownItems) {
        item->option()->state = QStyle::State_None;
        if (item->rect().contains(event->pos())) {
            item->option()->state = QStyle::State_MouseOver;
            if (d->currentHoveredItem != item) {
                item->timeLine()->setDirection(QTimeLine::Forward);
                item->timeLine()->start();
                if (d->currentHoveredItem) {
                    d->currentHoveredItem->timeLine()->setDirection(QTimeLine::Backward);
                    d->currentHoveredItem->timeLine()->start();
                }
                d->currentHoveredItem = item;
            }
        }
    }
    update();
}

void RaptorGraphicsView::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->currentHoveredItem) {
        d->currentHoveredItem->timeLine()->setDirection(QTimeLine::Backward);
        d->currentHoveredItem->timeLine()->start();
    }
    update();
}

void RaptorGraphicsView::slotAddFavorite(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    QString url(index.data(Qt::UserRole + 2).toString());

    if (url.isNull()) {
        return;
    }

    emit favoriteAddRequested(url);
}

void RaptorGraphicsView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void RaptorGraphicsView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    foreach (RaptorMenuItem * item, d->shownItems) {
	if (item->rect().contains(event->pos())) {

	    if (d->delegate->editorEvent(event, d->model, *item->option(), item->modelIndex())) { // let's give priority to the editor event
		break;
	    }

	    setRootIndex(item->modelIndex());
	    break;
	}
    }
}
