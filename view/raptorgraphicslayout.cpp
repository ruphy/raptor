/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorgraphicslayout.h"

#include "raptorgraphicsview.h"
#include "raptormenuitem.h"

#include <QEvent>

#include <KDebug>

class RaptorGraphicsLayout::Private
{
public:
    Private(RaptorGraphicsLayout *q) : q(q), view(0), topMargin(0), leftMargin(0), rightMargin(0), bottomMargin(0)
    {}
    ~Private()
    {}

    RaptorGraphicsLayout *q;
    RaptorGraphicsView *view;
    QList<RaptorMenuItem*> items;
    QList<RaptorMenuItem*> visibleItems;

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
}

RaptorGraphicsLayout::~RaptorGraphicsLayout()
{
    delete d;
}

void RaptorGraphicsLayout::setMenuItems(const QList<RaptorMenuItem*> &items)
{
    d->items.clear();
    d->visibleItems.clear();
    d->items << items;
}

QList<RaptorMenuItem*> RaptorGraphicsLayout::visibleItems() const
{
    return d->visibleItems;
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
    kDebug() << "";
    if (obj != d->view) {
        return false;
    }

    if (event->type() == QEvent::GraphicsSceneResize) {
        kDebug() << "re-layouting items";
        d->layoutItems();
        kDebug() << d->visibleItems;
        d->view->update();
        return true;
    }

    return false;
}

void RaptorGraphicsLayout::invalidate()
{
    d->layoutItems();
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
    RaptorMenuItem * oldFirst = 0;
    if (!visibleItems.isEmpty()) {
        oldFirst = visibleItems.first();
    }
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
            kDebug() << "Set item rect" << QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size));

            item->setRect(QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size)));
            //item->moveTo(QRectF(QPointF(sizesSum, topMargin), QSizeF(size, size)));
            sizesSum += size;

            if (sizesSum - item->rect().width() > rect.width()) {
                break;
            }
            visibleItems << item;
        }
        if (!visibleItems.contains(items.last())) {
            items.last()->setRect(QRectF(QPointF(-1 * size,topMargin), QSizeF(size, size)));
        }
//         if (oldFirst && !visibleItems.contains(oldFirst)) {
//             kDebug() << "Move the old first outside";
//             oldFirst->moveTo(QRectF(QPointF(!size, topMargin), QSizeF(size, size)));
// //             needsAnimation << oldFirst;
//         }

//         if (scrollTimeLine->state() == QTimeLine::Running) {
//             scrollTimeLine->stop();
//         }
//         scrollTimeLine->start();
    }

    else if (mode == RaptorGraphicsView::SingleApp) {
        RaptorMenuItem *item = items.first();
        item->setRect(QRectF(QPointF(0, 0), rect.size()));//Don't use rect for single-app, we don't need a margin here
        visibleItems << item;
    }

    else if (mode == RaptorGraphicsView::TwoApps) {
	qreal x = 0;
	for (int i = 0; i < 2; i++) {
            if (items.count() <= i) {
                break;
            }
	    RaptorMenuItem *item = items[i];
	    item->setRect(QRectF(QPointF(x, topMargin), QSizeF(rect.width() / 2, rect.height())));
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
	      item->setRect(QRectF(QPointF(0, y), QSizeF(rect.height(), rect.height() / 2)));
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
	      item->setRect(QRectF(QPointF(sizesSum, topMargin), QSizeF(rect.height(), rect.height())));
	      visibleItems << item;

	      sizesSum += rect.height();
	  }

	  int max = i + 2;
	  y = topMargin;
	  for (; i < max; i++) { // here we handle the last two items
	      RaptorMenuItem *item = items[i];
	      item->setRect(QRectF(QPointF(sizesSum, y), QSizeF(rect.height(), rect.height() / 2)));
	      y += rect.height() / 2;
              visibleItems << item;
	  }
    }

}
