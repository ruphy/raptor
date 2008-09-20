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

const int ANIMATION_DURATION = 150; // ms FIXME make me shorter

class RaptorItemDelegate::Private
{
    public:
        Private(RaptorItemDelegate *q):
                q(q),
                timeLine(new QTimeLine(ANIMATION_DURATION, q)),
                textColor(QColor()),
                p(0)
                {}
        ~Private()
                { delete p; }

    RaptorItemDelegate *q;
    QStyleOptionViewItemV4 optV4;
    const QAbstractItemView *view;
    QTimeLine *timeLine;
    int frame;
    QModelIndex index;
    QColor textColor;
    QPixmap *p;
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

    d->optV4.decorationSize = QSize(64, 64);

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        d->optV4.state &= ~QStyle::State_MouseOver; //this removes the mouseOver state in order to draw a nicer selection rect

        // here comes what should be animated
        if (d->timeLine->state() == QTimeLine::NotRunning && d->index != index) {

            d->index = index;

            d->timeLine->setFrameRange(0, 20);
            d->timeLine->start();
        }

//         QPixmap temp = *(d->p);
//         QPainter p(&temp);
//         p.setCompositionMode(QPainter::CompositionMode_Source);
//         p.drawPixmap(0, 0, *d->p);
//         p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
//         p.fillRect(temp.rect(), QColor(0, 0, 0, qreal(d->frame)*5.0/100.0));
//         p.end();
//         painter->drawPixmap(d->optV4.rect, temp);
        painter->save();
        painter->setOpacity(qreal(d->frame)*5.0/100.0);
        painter->drawPixmap(d->optV4.rect, *d->p);
        painter->restore();

    }

    if (d->optV4.state & QStyle::State_Selected) {
        painter->drawPixmap(d->optV4.rect, *d->p);
    }

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

    QStyledItemDelegate::paint(painter, d->optV4, index);

}

void RaptorItemDelegate::generateBgPixmap() const // TODO find a way to make this themable, preferrably via SVG.
{
    if (!d->p) { // it's an expensive operation, so let's keep a cached pixmap. TODO: profile and eventually share it across items. TODO cache QPixmap instead.
        QSize s(d->optV4.decorationSize.width()-14, d->optV4.decorationSize.height()-14); //FIXME: un-hardcode that 10 value (modify also below)
        QImage *i = new QImage(d->optV4.decorationSize, QImage::Format_ARGB32_Premultiplied);
        i->fill(0);
        QPainter p(i);
        QLinearGradient lg(0, 7, 0,  s.height());
        lg.setColorAt(0.0, QColor(255, 255, 255, 255));
        lg.setColorAt(1.0, QColor(255, 255, 255, 39));
        p.setBrush(lg);
        p.drawRect(7, 7, s.width(), s.height());

        expblur<16, 7>(*i, 8);

        p.end();

        d->p = new QPixmap(d->optV4.decorationSize);
        d->p->fill(Qt::transparent);
        
        QPainter pp(d->p);
        pp.setCompositionMode(QPainter::CompositionMode_Source);
        pp.drawPixmap(0, 0, QPixmap::fromImage(*i));
        pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        pp.fillRect(d->p->rect(), QColor(0, 0, 0, 125));
        pp.end();

        delete i;
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
