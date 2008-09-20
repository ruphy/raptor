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

#include "blur.cpp" //TODO: make this a function in Plasma::PaintUtils

const int ANIMATION_DURATION = 500; // ms FIXME make me shorter

class RaptorItemDelegate::Private
{
    public:
        Private(RaptorItemDelegate *q):
                q(q),
                timeLine(new QTimeLine(ANIMATION_DURATION, q)),
                textColor(QColor()),
				i(0)
                {}
        ~Private()
                { delete i; }

    RaptorItemDelegate *q;
    QStyleOptionViewItemV4 optV4;
    const QAbstractItemView *view;
    QTimeLine *timeLine;
    int frame;
    QModelIndex index;
    QColor textColor;
	QImage *i;
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
    initStyleOption(&d->optV4, index);
    d->view = qobject_cast<const QAbstractItemView*>(d->optV4.widget);

    generateBgPixmap(); // TODO Bg --> Background

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    d->optV4.decorationSize /= 2;

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        d->optV4.state &= ~QStyle::State_MouseOver; //this removes the mouseOver state in order to draw a nicer selection rect
        //d->optV4.state &= ~QStyle::State_Selected; //don't draw standard selection rect

        // here comes what should be animated
        if (d->timeLine->state() == QTimeLine::NotRunning && d->index != index) {
            //const int oldValue = d->optV4.decorationSize.width();
            //const int newValue = d->optV4.decorationSize.width()*2;

            d->index = index;

            d->timeLine->setFrameRange(0, 20);
            d->timeLine->start();
        }

        painter->save();
        painter->setOpacity(qreal(d->frame)*5.0/100.0); //TODO: make me faster - QPixmap hack
        painter->drawImage(d->optV4.rect, *d->i);
        painter->restore();

    }

    if (d->optV4.state & QStyle::State_Selected) {
        painter->drawImage(d->optV4.rect, *d->i);
    }

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

    QStyledItemDelegate::paint(painter, d->optV4, index);


}

void RaptorItemDelegate::generateBgPixmap() const // TODO find a way to make this themable, preferrably via SVG.
{
    if (!d->i) { // it's an expensive operation, so let's keep a cached pixmap. TODO: profile and eventually share it across items. TODO cache QPixmap instead.
//         d->pixmap = new QPixmap;
        d->i = new QImage(d->optV4.decorationSize, QImage::Format_ARGB32_Premultiplied);
        d->i->fill(0);
        QPainter p(d->i);
        p.setOpacity(0.41);
        QLinearGradient lg(0, 0, 0,  d->optV4.decorationSize.height());
        lg.setColorAt(0.0, QColor(255, 255, 255, 255));
        lg.setColorAt(1.0, QColor(255, 255, 255, 39));
        p.setBrush(lg);
        p.drawRect(0, 0, d->optV4.decorationSize.width(),  d->optV4.decorationSize.height());

        expblur<16, 7>(*d->i, 7);

//         d->pixmap->fromImage(i); FIXME use the right conversion
    }
}

void RaptorItemDelegate::animatePaint(int frame)
{
    d->frame = frame;
    d->view->viewport()->update(d->optV4.rect);
}

void RaptorItemDelegate::setTextColor(const QColor &color)
{
    d->textColor = color;
}
