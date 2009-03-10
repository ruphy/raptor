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
#include <QStyleOptionViewItemV4>

#include <KDebug>

#include <Plasma/Theme>

class RaptorGraphicsView::Private
{
public:
    Private(RaptorGraphicsView *q) : q(q), model(0), currentHoveredItem(0), xPress(0), scrollOffset(0)
    {
        delegate = new RaptorItemDelegate(q);
        delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
        topMargin = Plasma::Theme::defaultTheme()->fontMetrics().height();
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

    qreal xPress;
    qreal scrollOffset;

    int topMargin;
};

RaptorGraphicsView::RaptorGraphicsView(QGraphicsItem *parent) : QGraphicsWidget(parent), d(new Private(this))
{
    //setViewMode(RaptorGraphicsView::Normal);
    //setViewMode(RaptorGraphicsView::SingleApp);
    setViewMode(RaptorGraphicsView::TwoApps);
    //setViewMode(RaptorGraphicsView::Search);
    setAcceptHoverEvents(true);

    connect (d->delegate, SIGNAL(favoriteAddRequested(const QModelIndex &)), this, SLOT(slotAddFavorite(const QModelIndex &)));
}

RaptorGraphicsView::~RaptorGraphicsView()
{
    delete d;
}

void RaptorGraphicsView::reset()
{
    getItems();
    setupItems();
    update();
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

    //Paint description, FIXME: Change DataRole
    if ((d->currentHoveredItem || viewMode() == RaptorGraphicsView::SingleApp) && !d->shownItems.isEmpty()) {
        RaptorMenuItem * item = viewMode() == RaptorGraphicsView::SingleApp ? d->shownItems.first() : d->currentHoveredItem;

        painter->save();

        painter->setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
        painter->drawText(contentsRect(), Qt::AlignRight, item->modelIndex().data(Qt::DisplayRole).toString());

        painter->restore();
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
    if (d->items.isEmpty()) {
        d->shownItems.clear();
        return;
    }
    d->delegate->setViewMode((RaptorItemDelegate::ViewMode)viewModeFromItemCount());
    // NOTE: for each view mode we should setup items individually
    // WARNING: we suppose a horizontal view
    // TODO: remove tabs

    d->shownItems.clear();
    ViewMode mode = viewMode();

    QRectF rect = contentsRect();
    rect.setSize(QSizeF(rect.width(), rect.height() - d->topMargin));
    rect.moveBottom(d->topMargin);

    if (mode == RaptorGraphicsView::Normal) {
        qreal sizesSum = 0;
        qreal size = rect.height();
        int i = 0;
        foreach (RaptorMenuItem *item, d->items) {
            item->setRect(QRectF(QPointF(sizesSum, d->topMargin), QSizeF(size, size)));
            sizesSum += size;
            if (sizesSum + item->rect().width() < rect.x()) {
                continue;
            }
            if ( sizesSum - item->rect().width() > rect.width() ) {
                break;
            }
            d->shownItems << item;
            i++;
        }
    }

    else if (mode == RaptorGraphicsView::SingleApp) {
        RaptorMenuItem *item = d->items.first();
        item->setRect(QRectF(QPointF(0, 0), contentsRect().size()));//Don't use rect for single-app, we don't need a margin here
        d->shownItems << item;
    }

    else if (mode == RaptorGraphicsView::TwoApps) {
	qreal x = 0;
	for (int i = 0; i < 2; i++) {
            if (d->items.count() <= i) {
                break;
            }
	    RaptorMenuItem *item = d->items[i];
	    item->setRect(QRectF(QPointF(x, d->topMargin), QSizeF(rect.width() / 2, rect.height())));
	    x += rect.width() / 2;
            d->shownItems << item;
	}
     }

     else if (mode == RaptorGraphicsView::Search) {
	  qreal sizesSum = 0;
	  int i = 0;
	  qreal y = d->topMargin;

	  for (; i < 2; i++) { // we place the first two items half sized and in column
	      RaptorMenuItem *item = d->items[i];
	      item->setRect(QRectF(QPointF(rect.height() / 4, y), QSizeF(rect.height() / 2, rect.height() / 2)));
	      y += rect.height() / 2;
              d->shownItems << item;
	  }
	  sizesSum += contentsRect().height();

	  for (; i < d->items.count(); i++) { // now we take care of left items
	      if (sizesSum > rect.width() - (rect.height())) {
		  sizesSum = rect.width() - (rect.height());
		  break;
	      }

	      if ( d->items.count() - i == 2 ) {
		  break;
	      }

	      RaptorMenuItem *item = d->items[i]; 
	      item->setRect(QRectF(QPointF(sizesSum, d->topMargin), QSizeF(rect.height(), rect.height())));
	      d->shownItems << item;

	      sizesSum += rect.height();
	  }

	  int max = i + 2;
	  y = d->topMargin;
	  for (; i < max; i++) { // here we handle the last two items
	      RaptorMenuItem *item = d->items[i];
	      item->setRect(QRectF(QPointF(sizesSum + rect.height() / 4, y), QSizeF(rect.height() / 2, rect.height() / 2)));
	      y += rect.height() / 2;
              d->shownItems << item;
	  }
    }

}

void RaptorGraphicsView::scrollItems()
{
    if (viewMode() == RaptorGraphicsView::Search) {
        if (abs(d->scrollOffset) < 10) {
            return;
        }
        if (d->scrollOffset > 0) {
            scrollLeft();
        } else {
            scrollRight();
        }
        return;
    }

    foreach (RaptorMenuItem *item, d->shownItems) {
        QRectF rect = item->rect();
        rect.translate(d->scrollOffset, 0);
        item->setRect(rect);
    }

    update();
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
    if (viewMode() == RaptorGraphicsView::SingleApp) {
        return;
    }
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
    if (viewMode() == RaptorGraphicsView::SingleApp) {
        return;
    }
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
    if (viewMode() == RaptorGraphicsView::SingleApp) {
        return;
    }
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

    emit favoriteClicked(url);
}

void RaptorGraphicsView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    kDebug() << "View mode is:" << viewMode() << RaptorGraphicsView::Normal;
    if (event->button() == Qt::RightButton && viewMode() != RaptorGraphicsView::SingleApp) {
        setViewMode(RaptorGraphicsView::SingleApp);//FIXME: Scroll to current item
    } else if (viewMode() != RaptorGraphicsView::Normal) {
        kDebug() << "Yeah do it!";
        setViewMode(RaptorGraphicsView::Normal);//FIXME: Check whether we have a search entry for example...
    }
    d->xPress = event->pos().x();
    event->accept();
}

void RaptorGraphicsView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    kDebug();
    foreach (RaptorMenuItem * item, d->shownItems) {
	if (item->rect().contains(event->pos())) {

	    if (d->delegate->editorEvent(event, d->model, *item->option(), item->modelIndex())) { // let's give priority to the editor event
		break;
	    }

            if (d->xPress == event->pos().x()) {
	        setRootIndex(item->modelIndex());
            }
	    break;
	}
    }
}

void RaptorGraphicsView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    d->scrollOffset = event->pos().x() - event->lastPos().x();

    scrollItems();
}

RaptorGraphicsView::ViewMode RaptorGraphicsView::viewModeFromItemCount()
{
    switch (d->items.count()) {
        case 1:
            return RaptorGraphicsView::SingleApp;
        case 2:
            return RaptorGraphicsView::TwoApps;
        case 3:
        case 4:
        case 5:
            return RaptorGraphicsView::Normal;
        default:
            return RaptorGraphicsView::Search;
    }
}
