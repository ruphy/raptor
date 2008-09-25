/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "raptordescriptiondelegate.h"

//Qt
#include <QPainter>
#include <QSize>
#include <QStyleOptionViewItemV4>

//KDE
#include <KDebug>

class RaptorDescriptionDelegate::Private
{
    public:
        Private(RaptorDescriptionDelegate *q):
                q(q),
                textColor(QColor())
                {}
        ~Private()
                {}

    RaptorDescriptionDelegate *q;
    QColor textColor;
    QStyleOptionViewItemV4 optV4;
};

RaptorDescriptionDelegate::RaptorDescriptionDelegate(QObject *parent) : QStyledItemDelegate(parent),
                                                                        d(new Private(this))
{}

RaptorDescriptionDelegate::~RaptorDescriptionDelegate()
{}

void RaptorDescriptionDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    if (!index.isValid()) {
        return;
    }

    d->optV4 = option;
    initStyleOption(&d->optV4, index);

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setClipRect(d->optV4.rect);
    painter->setPen(Qt::NoPen);

    //WARNING: we assume to have a wide rect to draw icon + description.
    //TODO: add the method setMode to the View in order to manage the description delegate

    QPixmap pixmapDecoration = d->optV4.icon.pixmap(d->optV4.decorationSize);

    QRect iconRect = d->optV4.rect;
    iconRect.setSize(d->optV4.decorationSize);

    painter->drawPixmap(iconRect, pixmapDecoration);

    painter->setPen(d->optV4.palette.color(QPalette::Text));

    QRect descriptionRect = d->optV4.rect;
    descriptionRect.translate( iconRect.width(), 0);
    descriptionRect.setSize(QSize(d->optV4.rect.width() - iconRect.width(), descriptionRect.height()));

    //TODO: define the role to use for description
    painter->drawText(descriptionRect, Qt::AlignHCenter, index.data(Qt::DisplayRole).toString());

    painter->restore();
}

void RaptorDescriptionDelegate::setTextColor(const QColor &color)
{
    d->textColor = color;
}
