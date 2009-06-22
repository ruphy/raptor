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
#include "raptorgraphicslayout.h"

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
    { qDeleteAll(items); items.clear(); }

    RaptorGraphicsView *q;
    RaptorGraphicsLayout *layout;

    QAbstractItemModel *model;
    QModelIndex rootIndex;

    QList<RaptorMenuItem*> items;

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
    //setViewMode(RaptorGraphicsView::TwoApps);
    //setViewMode(RaptorGraphicsView::Search);

    d->layout = new RaptorGraphicsLayout(this);
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
    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
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
    return d->layout->visibleItems();
}

void RaptorGraphicsView::setRootIndex(const QModelIndex &index)
{
    if (index == d->rootIndex) {
        return;
    }
    if (!index.data(Qt::UserRole + 2).isNull()) {
        emit applicationClicked(index); //Qt::UserRole + 2 is Kickoff::UrlRole
        return;
    }
    d->rootIndex = index;
    getItems();
    setViewMode(viewModeFromItemCount());
    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
    update();
    emit enteredItem(d->rootIndex);
}

void RaptorGraphicsView::scrollRight()
{
    if (d->items == shownItems())
        return;

    RaptorMenuItem *item = d->items.first();
    d->items.removeAll(item);
    d->items.append(item);

    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
    //update();
}

void RaptorGraphicsView::scrollLeft()
{
    if (d->items == shownItems())
        return;

    RaptorMenuItem *item = d->items.last();
    d->items.removeAll(item);
    d->items.prepend(item);

    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
    //update();
}

QAbstractItemModel* RaptorGraphicsView::model()
{
    return d->model;
}

void RaptorGraphicsView::setModel(QAbstractItemModel *model)
{
    if (d->model) {
        disconnect(d->model, SIGNAL(rowsInserted(QModelIndex, int, int)), 0, 0);
        disconnect(d->model, SIGNAL(rowsRemoved(QModelIndex, int, int)), 0, 0);
        disconnect(d->model, SIGNAL(modelReset()), 0, 0);
    }
    d->model = model;
    connect(d->model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(addRows(QModelIndex, int, int)));
    connect(d->model, SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(removeRows(QModelIndex, int, int)));
    connect(d->model, SIGNAL(modelReset()), SLOT(reset()));
    d->rootIndex = QModelIndex();

    setViewMode(viewModeFromItemCount());

    getItems();

    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
    update();
}

void RaptorGraphicsView::setViewMode(ViewMode viewMode)
{
    d->delegate->setViewMode((RaptorItemDelegate::ViewMode)viewMode);
}

RaptorGraphicsView::ViewMode RaptorGraphicsView::viewMode()
{
    return (RaptorGraphicsView::ViewMode)d->delegate->viewMode();
}

void RaptorGraphicsView::addRows(const QModelIndex &parent, int start, int end)
{
    kDebug();
    for (int i = start; i != end; i++) {
        d->items.append(new RaptorMenuItem(parent.child(i, 0)));
    }

    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
    //update();
}

void RaptorGraphicsView::removeRows(const QModelIndex &parent, int start, int end)
{
    kDebug();
    QList<RaptorMenuItem*> toRemove;
    for (int i = start; i != end; i++) {
        toRemove.append(d->items.at(i));
    }
    foreach (RaptorMenuItem * item, toRemove) {
        d->items.removeAll(item);
    }

    d->layout->setMenuItems(d->items);
    d->layout->invalidate();
    //update();
}

void RaptorGraphicsView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->setClipRect(contentsRect());
//     painter->fillRect(rect(), Qt::green);

    foreach (RaptorMenuItem *item, d->layout->visibleItems()) {
        //kDebug() << "Paint" << item->modelIndex().data(Qt::DisplayRole) << "at" << item->option()->rect;
        //kDebug() << "Paint" << item->modelIndex().data(Qt::DisplayRole) << "to" << item->rect();
        d->delegate->paint(painter, *item->option(), item->modelIndex());
    }
//     if (d->scrollTimeLine->currentFrame() != 20) {
//         foreach (RaptorMenuItem *item, d->needsAnimation) {
//             d->delegate->paint(painter, *item->option(), item->modelIndex());
//         }
//     }

    //Paint description, FIXME: Change DataRole
    if ((d->currentHoveredItem || viewMode() == RaptorGraphicsView::SingleApp) && !d->layout->visibleItems().isEmpty()) {
        RaptorMenuItem * item = viewMode() == RaptorGraphicsView::SingleApp ? d->layout->visibleItems().first() : d->currentHoveredItem;

        painter->save();

        painter->setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
        painter->drawText(contentsRect(), Qt::AlignRight, item->modelIndex().data(Qt::DisplayRole).toString());
        //painter->fillRect(contentsRect(), Qt::green);

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

    foreach (RaptorMenuItem *item, shownItems()) {
        QRectF rect = item->rect();
        rect.translate(d->scrollOffset, 0);
        item->setRect(rect);
    }

    update();
}

void RaptorGraphicsView::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (viewMode() == RaptorGraphicsView::SingleApp) {
        return;
    }
    foreach (RaptorMenuItem *item, shownItems()) {
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
    foreach (RaptorMenuItem *item, shownItems()) {
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
    foreach (RaptorMenuItem * item, shownItems()) {
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
    //return RaptorGraphicsView::TwoApps;
    /*
    switch (d->items.count()) {
        case 1:
            return RaptorGraphicsView::SingleApp;
        case 2:
            return RaptorGraphicsView::TwoApps;
        case 3:
        case 4:
        case 5:*/
            return RaptorGraphicsView::Normal;
        /*default:
            return RaptorGraphicsView::Search;
    }*/
}
