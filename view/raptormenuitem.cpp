/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptormenuitem.h"
#include "raptoritemdelegate.h"

#include <QStyleOptionViewItem>
#include <QPainter>
#include <QModelIndex>

#include <KDebug>

class RaptorMenuItem::Private
{
public:
    Private(const QModelIndex &index, RaptorMenuItem *q) : q(q), index(index), option(new QStyleOptionViewItem)
    {
    }

    RaptorMenuItem *q;
    QModelIndex index;
    QRectF rect;
    QStyleOptionViewItem *option;
};

RaptorMenuItem::RaptorMenuItem(const QModelIndex &index, QObject *parent) : QObject(parent) , d(new Private(index, this))
{
}

RaptorMenuItem::~RaptorMenuItem()
{
    delete d;
}

QRectF RaptorMenuItem::rect() const
{
    return d->rect;
}

void RaptorMenuItem::setRect(const QRectF &rect)
{
    kDebug() << "setting rect" << rect;
    d->rect = rect;
    d->option->rect = rect.toRect();
    d->option->decorationSize = d->option->rect.size();
}

void RaptorMenuItem::moveBy(float dx, float dy)
{
    d->rect.translate(dx, dy);
}

QModelIndex RaptorMenuItem::modelIndex() const
{
    return d->index;
}

QStyleOptionViewItem* RaptorMenuItem::option()
{
    return d->option;
}
