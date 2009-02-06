/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>

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

class RaptorMenuItem::Private
{
public:
    Private(RaptorMenuItem *q, RaptorItemDelegate * del) : q(q), delegate(del)
    {}

    RaptorMenuItem *q;
    RaptorItemDelegate *delegate;
    QModelIndex index;
    QRectF rect;
};

RaptorMenuItem::RaptorMenuItem(QModelIndex index, RaptorItemDelegate * delegate, QObject *parent) : QObject(parent) , d(new Private(this, delegate))
{
}

RaptorMenuItem::~RaptorMenuItem()
{
    delete d;
}

void RaptorMenuItem::paint(QPainter *painter, const QStyleOptionViewItem &option)
{
    QStyleOptionViewItem opt = option;

    if (!d->rect.isNull()) {
        opt.rect = d->rect.toRect();
    } else {
        d->rect = opt.rect;
    }

    d->delegate->paint(painter, opt, d->index);
}

QRectF RaptorMenuItem::rect() const
{
    return d->rect;
}

void RaptorMenuItem::setRect(const QRectF &rect)
{
    d->rect = rect;
}

void RaptorMenuItem::moveBy(float dx, float dy)
{
    d->rect.translate(dx, dy);
}
