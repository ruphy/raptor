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
#include "raptorgraphicsview.h"

#include <QStyleOptionViewItem>
#include <QPainter>
#include <QModelIndex>
#include <QTimeLine>

#include <KDebug>
#include <KLocale>

#include <Plasma/Theme>

const int DURATION = 150;
const int FRAMES = 20;

class RaptorMenuItem::Private
{
public:
    Private(const QModelIndex &index, RaptorGraphicsView *p, RaptorMenuItem *q) : q(q), index(index), option(new QStyleOptionViewItem), view(p), frame(0)
    {
        lastUsed = i18n("20 Minutes");//FIXME: Replace with proper last used info :)
        lastUsedWidth = Plasma::Theme::defaultTheme()->fontMetrics().width(lastUsed);
    }
    ~Private()
    {
        delete option;
    }

    RaptorMenuItem *q;
    QModelIndex index;
    QRectF rect;
    QRectF finalRect;
    QStyleOptionViewItem *option;
    QTimeLine *timeLine;
    RaptorGraphicsView *view;
    QString lastUsed;
    int lastUsedWidth;
    int frame;

    void calculateDecorationSize();
    void calculateRect();
};

RaptorMenuItem::RaptorMenuItem(const QModelIndex &index, RaptorGraphicsView *parent) : QObject(parent) , d(new Private(index, parent, this))
{
    d->timeLine = new QTimeLine(DURATION, this);
    d->timeLine->setFrameRange(0, 20);
    connect(d->timeLine, SIGNAL(frameChanged(int)), SLOT(update()));
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
    kDebug() << "SET OUR RECT TO" << rect << "THE ORIGINAL WAS:" << d->rect;
    if (d->rect == rect) {
        return;
    }
    if (d->rect.isNull()) {
        kDebug() << "RECT IS NULL" << rect;;
        d->rect = rect;
        d->option->rect = rect.toRect();
        d->calculateDecorationSize();
        return;
    }
    d->finalRect = rect;
}

void RaptorMenuItem::setFrame(int frame)
{
    kDebug() << frame;
    d->frame = frame;
    d->calculateRect();
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

void RaptorMenuItem::Private::calculateRect()
{
    //kDebug() << "ITEM IS" << q->modelIndex().data(Qt::DisplayRole) << "FINAL RECT IS" << finalRect << "WE'RE AT" << option->rect;
    if (option->rect == finalRect) {
        kDebug() << "WE'RE AT FINAL RECT";
        rect = option->rect;
        return;
    }
    kDebug() << "Item:" << index.data(Qt::DisplayRole) << "Original rect is:" << rect << "Add that shit:" << rect.x() + (finalRect.x() - rect.x()) / FRAMES * frame << "Current rect is:" << option->rect;
    QRect rect = option->rect;
    rect.setSize(finalRect.size().toSize());
    rect.setX(rect.x() + (finalRect.x() - rect.x()) / FRAMES * frame);
    //QRectF rect(QPoint(rect.x() + (finalRect.x() - rect.x()) / FRAMES * frame, option->rect.y()), finalRect.size());
    option->rect = rect;
}

void RaptorMenuItem::Private::calculateDecorationSize()
{
    int size = rect.size().width();

    if (size < 16) {
        option->decorationSize = QSize(8, 8);
    } else if (size >= 16 || size < 22) {
        option->decorationSize = QSize(16, 16);
    } else if (size >= 22 || size < 32) {
        option->decorationSize = QSize(22, 22);
    } else if (size >= 32 || size < 48) {
        option->decorationSize = QSize(32, 32);
    } else if (size >= 48 || size < 64) {
        option->decorationSize = QSize(48, 48);
    } else if (size >= 64 || size < 128) {
        option->decorationSize = QSize(64, 64);
    } else if (size >= 128 || size < 256) {
        option->decorationSize = QSize(128, 128);
    } else {
        option->decorationSize = QSize(256, 256);
    }
}

QTimeLine * RaptorMenuItem::timeLine()
{
    return d->timeLine;
}

void RaptorMenuItem::update()
{
    d->view->update(d->rect);
}

QString RaptorMenuItem::lastUsed() const
{
    //TODO: Check whether the lastused text changed in the modelindex later and recalculate the size then...
    return d->lastUsed;
}

int RaptorMenuItem::lastUsedWidth()
{
    return d->lastUsedWidth;
}
