/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
//Local
#include "raptoritemsview.h"

#include <QTimeLine>
#include <QScrollBar>

const int SMOOTH_SCROLL_DURATION = 250; // ms

class RaptorItemsView::RaptorItemsViewPrivate
{

public:
    RaptorItemsViewPrivate() :
    q(q),
    timeLine(new QTimeLine(SMOOTH_SCROLL_DURATION, q))
    {}

    RaptorItemsView *q;
    Qt::Orientation orientation;
    QTimeLine *timeLine;
};


RaptorItemsView::RaptorItemsView(QWidget *parent) : QListView(parent),
                                                    d(new RaptorItemsViewPrivate())
{
    setWrapping(false);
    setMouseTracking(true);
    setUniformItemSizes(true);
    setViewMode(QListView::IconMode);

    setFrameShape(QFrame::NoFrame);

    //TODO: use Plasma::Style to draw scrollbars and maybe use round corners to
    //      beautify the view...
    
}

RaptorItemsView::~RaptorItemsView()
{}

void RaptorItemsView::setOrientation(Qt::Orientation value)
{
    d->orientation = value;
    
    if (d->orientation == Qt::Vertical) {
        setFlow(QListView::TopToBottom);
        disconnect(d->timeLine, SIGNAL(frameChanged(int)), horizontalScrollBar(), SLOT(setValue(int)));
        connect(d->timeLine, SIGNAL(frameChanged(int)), verticalScrollBar(), SLOT(setValue(int)));
    } else {
        setFlow(QListView::LeftToRight);
        disconnect(d->timeLine, SIGNAL(frameChanged(int)), verticalScrollBar(), SLOT(setValue(int)));
        connect(d->timeLine, SIGNAL(frameChanged(int)), horizontalScrollBar(), SLOT(setValue(int)));
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