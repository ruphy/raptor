/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "raptoritemdelegate.h"

//Qt
#include <QPainter>
#include <QTimeLine>
#include <QSize>
#include <QStyleOptionViewItemV4>
#include <QAbstractItemView>

//KDE
#include <KDebug>


const int ANIMATION_DURATION = 150; // ms

class RaptorItemDelegate::Private
{
    public:
        Private(RaptorItemDelegate *q):
                q(q),
                timeLine(new QTimeLine(ANIMATION_DURATION, q)),
                textColor(QColor())

                {}
        ~Private()
                {}

    RaptorItemDelegate *q;
    QStyleOptionViewItemV4 optV4;
    const QAbstractItemView *view;
    QTimeLine *timeLine;
    int animatedSize;
    QModelIndex index;
    QColor textColor;
};

RaptorItemDelegate::RaptorItemDelegate(QObject *parent) : QStyledItemDelegate(parent),
                                                          d(new Private(this))
{
    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(animatePaint(int)));
}

RaptorItemDelegate::~RaptorItemDelegate()
{
     delete d;
}

void RaptorItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    if (!index.isValid()) {
        return;
    }

    d->optV4 = option;
    initStyleOption(&d->optV4 ,index);
    d->view = qobject_cast<const QAbstractItemView*>(d->optV4.widget);


    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    d->optV4.decorationSize /= 2;

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {

        // here comes what should be animated
        if (d->timeLine->state() == QTimeLine::NotRunning && d->index != index) {
            const int oldValue = d->optV4.decorationSize.width();
            const int newValue = d->optV4.decorationSize.width()*2;

            d->index = index;

            d->timeLine->setFrameRange(oldValue, newValue);
            d->timeLine->start();
        }
        d->optV4.decorationSize = QSize(d->animatedSize, d->animatedSize);

    }

    if (d->optV4.state & QStyle::State_Selected) {
        d->optV4.decorationSize *= 2;
    }

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

//     kDebug()<<d->optV4.decorationSize;
    QStyledItemDelegate::paint(painter, d->optV4, index);


}

void RaptorItemDelegate::animatePaint(int frame)
{
    d->animatedSize = frame;
    d->view->viewport()->update(d->optV4.rect);
}

void RaptorItemDelegate::setTextColor(const QColor &color)
{
    d->textColor = color;
}
