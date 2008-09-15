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

//KDE
#include <KDebug>
// #include <KFileItemDelegate>
//

const int ANIMATION_DURATION = 1000; // ms

class RaptorItemDelegate::Private
{
    public:
        Private(RaptorItemDelegate *q):
                q(q)
                //fileItemDelegate(new KFileItemDelegate(q))
                {}
        ~Private()
                {}

    RaptorItemDelegate *q;
    QStyleOptionViewItem opt;
    QPainter *painter;
    QModelIndex index;
    //KFileItemDelegate *fileItemDelegate;
};

RaptorItemDelegate::RaptorItemDelegate(QObject *parent) : QStyledItemDelegate(parent),
                                                          d(new Private(this))
{
}

RaptorItemDelegate::~RaptorItemDelegate()
{
     delete d;
}

void RaptorItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    kDebug()<<"painting";
    if (!index.isValid()) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    QStyleOptionViewItem opt = option;
    opt.decorationSize /= 2;

    d->painter = painter;
    d->index = index;
    d->opt = opt;

    if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_Selected) {

        opt.decorationSize *= 2;

    }
        QStyledItemDelegate::paint(painter, opt, index);

//     d->fileItemDelegate->paint(painter, option, index);

}

