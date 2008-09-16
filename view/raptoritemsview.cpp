/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

//Local
#include "raptoritemsview.h"
#include "engine/kickoff/applicationmodel.h"
#include "engine/kickoff/models.h"

//KDE
#include <KDebug>
#include <KUrl>

//Qt
#include <QTimeLine>
#include <QScrollBar>
#include <QFontMetrics>
#include <QApplication>


const int SMOOTH_SCROLL_DURATION = 250; // ms

class RaptorItemsView::RaptorItemsViewPrivate
{

public:
    RaptorItemsViewPrivate(RaptorItemsView *q) :
    q(q),
    timeLine(new QTimeLine(SMOOTH_SCROLL_DURATION, q))
    {}

    RaptorItemsView *q;
    Qt::Orientation orientation;
    QTimeLine *timeLine;
};


RaptorItemsView::RaptorItemsView(QWidget *parent) : QListView(parent),
                                                    d(new RaptorItemsViewPrivate(this))
{
    setWrapping(false);
    setMouseTracking(true);
    //setUniformItemSizes(true);
    setViewMode(QListView::IconMode);

    setFrameShape(QFrame::NoFrame);

    //WARNING: maybe those following 2 lines should be called from the applet implementation.
//     viewport()->setAttribute(Qt::WA_NoSystemBackground);
//     setAttribute(Qt::WA_NoSystemBackground);

    setOrientation(Qt::Horizontal);

    setAutoScroll(true);

    //TODO: use Plasma::Style to draw scrollbars and maybe use round corners to
    //      beautify the view...

    connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(smoothScrollTo(const QModelIndex &)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(enterItem(const QModelIndex &)));
    
}

RaptorItemsView::~RaptorItemsView()
{
    delete d;
}

void RaptorItemsView::setOrientation(Qt::Orientation value)
{
    d->orientation = value;
    
    if (d->orientation == Qt::Vertical) {
        setFlow(QListView::TopToBottom);
        disconnect(d->timeLine, SIGNAL(frameChanged(int)), horizontalScrollBar(), SLOT(setValue(int)));
        connect(d->timeLine, SIGNAL(frameChanged(int)), verticalScrollBar(), SLOT(setValue(int)));
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // FIXME: this does not seem to work
    } else {
        setFlow(QListView::LeftToRight);
        disconnect(d->timeLine, SIGNAL(frameChanged(int)), verticalScrollBar(), SLOT(setValue(int)));
        connect(d->timeLine, SIGNAL(frameChanged(int)), horizontalScrollBar(), SLOT(setValue(int)));
        setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel); // FIXME: this does not seem to work
    }
}

int RaptorItemsView::getNewScrollValue(const QRect &rect)
{
    int currentValue;
    if (d->orientation == Qt::Horizontal) {
        currentValue = horizontalScrollBar()->value();
        currentValue += rect.left() + (rect.width() - viewport()->width())/2;
    } else {
        currentValue = verticalScrollBar()->value();
        currentValue += rect.top() + (rect.height() - viewport()->height())/2;
    }

    return currentValue;
}

void RaptorItemsView::smoothScrollTo(const QModelIndex &index)
{
   if(!index.isValid())
      return;

   const QRect rect = visualRect(index);

   int oldValue;
   int newValue;

   if (d->orientation == Qt::Horizontal) {
      oldValue = horizontalScrollBar()->value();
   } else {
      oldValue = verticalScrollBar()->value();
   }

   newValue = getNewScrollValue(rect);

   if (d->timeLine->state() == QTimeLine::Running) {
      d->timeLine->stop();
   }

   d->timeLine->setFrameRange(oldValue, newValue);
   d->timeLine->start();
}

void RaptorItemsView::resizeEvent(QResizeEvent *event)
{
    QListView::resizeEvent(event);

    QFontMetrics fm(QApplication::font());

    if (d->orientation == Qt::Vertical) {
        int size = viewport()->width();
        setGridSize(QSize(size, size));
        int iconSize = (size - fm.height());
        setIconSize(QSize(iconSize, iconSize));

    } else {
        int size = viewport()->height();
        setGridSize(QSize(size, size));
        int iconSize = (size - fm.height());
        setIconSize(QSize(iconSize, iconSize));
    }
}

void RaptorItemsView::enterItem(const QModelIndex &index)
{
    if ( model()->hasChildren(index)) {
        setRootIndex(index);
    } else {
        emit applicationClicked(KUrl(model()->data(index, Kickoff::UrlRole).toString()));
    }
}

void RaptorItemsView::browseBack()
{
    setRootIndex( model()->parent(rootIndex()) );
}

void RaptorItemsView::setModel(Kickoff::ApplicationModel *model)
{
    QListView::setModel(model);
}

Kickoff::ApplicationModel* RaptorItemsView::model()
{
    return static_cast<Kickoff::ApplicationModel *>(QListView::model());
}

void RaptorItemsView::hideScrollBars()
{
    horizontalScrollBar()->hide();
    verticalScrollBar()->hide();
}

void RaptorItemsView::showScrollBars()
{
    horizontalScrollBar()->show();
    verticalScrollBar()->show();
}
