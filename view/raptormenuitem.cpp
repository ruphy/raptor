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
#include "engine/nepomuk/nepomukmodel.h"

#include <QStyleOptionViewItem>
#include <QPainter>
#include <QModelIndex>
#include <QTimeLine>

#include <KDebug>
#include <KLocale>

#include <Plasma/Theme>

const int DURATION = 150;

class RaptorMenuItem::Private
{
public:
    Private(const QModelIndex &index, RaptorGraphicsView *p, RaptorMenuItem *q) : q(q), index(index), option(new QStyleOptionViewItem), view(p), value(0)
    {
        //lastUsed = i18n("20 Minutes");//FIXME: Replace with proper last used info :)
        //lastUsedWidth = Plasma::Theme::defaultTheme()->fontMetrics().width(lastUsed);
    }
    ~Private()
    {
        delete option;
    }

    RaptorMenuItem *q;
    QModelIndex index;

    QRectF rect;
    QRectF initialRect;
    QRectF finalRect;

    QStyleOptionViewItem *option;
    QTimeLine *timeLine;
    RaptorGraphicsView *view;
    QString lastUsed;
    int lastUsedWidth;
    qreal value;

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
    kDebug() << "dying" << this;
    delete d;
}

QRectF RaptorMenuItem::rect() const
{
    return d->rect;
}

void RaptorMenuItem::setRect(const QRectF &rect)
{
    if (d->rect == rect) {
        return;
    }

    d->rect = rect;
    d->option->rect = rect.toRect();
    d->calculateDecorationSize();
}

void RaptorMenuItem::moveTo(const QRectF &rect)
{
    if (d->rect == rect) {
        return;
    }

    if (d->rect.isNull()) {
        setRect(rect);
        return;
    }
    kDebug() << "Move" << d->index.data(Qt::DisplayRole) << "to" << rect;

    //d->rect.setSize(rect.size());//TODO: Animate size changes
    //d->option->rect.setSize(rect.size().toSize());
    d->initialRect = d->rect;
    d->finalRect = rect;
}

void RaptorMenuItem::setAnimationValue(qreal value)
{
    if (d->finalRect == d->initialRect) {
        return;
    }
    //kDebug() << value;
    d->value = value;
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
    if (value == 1) {
        rect = finalRect;
        option->rect = rect.toRect();
        return;
    }

    qreal xTranslation = (finalRect.x() - initialRect.x()) * value;
    qreal yTranslation = (finalRect.y() - initialRect.y()) * value;
    kDebug() << index.data().toString() << "X-Translation is:" << xTranslation << "Animation value is:" << value << "Initial Rect:" << initialRect;
    rect = initialRect.translated(xTranslation, yTranslation);
    rect.setSize(QSizeF(initialRect.width() + (finalRect.width() - initialRect.width()) * value, initialRect.height() + (finalRect.height() - initialRect.height()) * value));
    option->rect = rect.toRect();
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
    //FIXME: Caching?
    return d->index.data(Raptor::NepomukModel::LastLaunchedRole).toDateTime().toString();
}

int RaptorMenuItem::lastUsedWidth()
{
    return Plasma::Theme::defaultTheme()->fontMetrics().width(d->index.data(Raptor::NepomukModel::LastLaunchedRole).toDateTime().toString());
}
