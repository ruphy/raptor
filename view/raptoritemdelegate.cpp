/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2008 Riccardo Iaconelli <riccardo@kde.org>

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
#include <QApplication>
#include <QGraphicsWidget>

//KDE
#include <Plasma/Theme>
#include <KDebug>

#include "blur.cpp" //TODO: make this a function in Plasma::PaintUtils

// FIXME: use Animator, for a shared timer.
const int ANIMATION_DURATION = 200; // will need to be made shorter once we have keyboard navigation.

class RaptorItemDelegate::Private
{
    public:
        Private(QGraphicsWidget *view, RaptorItemDelegate *q):
                q(q),
                view(view),
                timeLine(new QTimeLine(ANIMATION_DURATION, q)),
                textColor(QColor()),
                p(0),
                mode(RaptorItemDelegate::Normal)
                {}

        ~Private()
                { delete p; delete timeLine; }

    RaptorItemDelegate *q;

    QStyleOptionViewItemV4 optV4;
    QGraphicsWidget *view;

    QTimeLine *timeLine;
    int frame;

    QModelIndex index;

    QColor textColor;
    QPixmap *p;

    RaptorItemDelegate::ViewMode mode;
};

RaptorItemDelegate::RaptorItemDelegate(QGraphicsWidget *parent) : QStyledItemDelegate(parent),
                                                          d(new Private(parent, this))
{
//     connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(animatePaint(int)));
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

    switch (d->mode) {
        case RaptorItemDelegate::Normal :
            drawNormalWay(painter, option, index);
            break;
        case RaptorItemDelegate::SingleApp :
            drawSingleAppWay(painter, option, index);
            break;
        default :
            drawNormalWay(painter, option, index);
            break;
    }
}

void RaptorItemDelegate::drawNormalWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const qreal textMargin = 3;
    const qreal iconSize = 64;

    d->optV4 = option;
    initStyleOption(&d->optV4, index);

    if (!d->p) {
        generateBgPixmap(d->optV4.decorationSize); // TODO Bg --> Background
    }
    
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    //d->optV4.decorationSize = QSize(64, 64);

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        d->optV4.state &= ~QStyle::State_MouseOver; //this removes the mouseOver state in order to draw a nicer selection rect

        // here comes what should be animated
//         if (d->timeLine->state() == QTimeLine::NotRunning && d->index != index) {
// 
//             d->index = index;
// 
//             d->timeLine->setFrameRange(0, 20);
//             d->timeLine->start();
//         }

        QPixmap temp = (d->p)->size();//d->optV4
        QPainter p(&temp);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawPixmap(0, 0, *d->p);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.fillRect(temp.rect(), QColor(0, 0, 0, 100/*qreal(d->frame)*5.0/100.0*/));
        p.end();
//         painter->drawPixmap(d->optV4.rect, temp);
//         painter->save();
//         painter->setOpacity(qreal(d->frame)*5.0/100.0);
        QPoint topLeft(d->optV4.rect.x()+(d->optV4.decorationSize.width()-d->p->width())/2,
                       d->optV4.rect.y()+(d->optV4.decorationSize.height()-d->p->height())/2);
        QRect pixRect(topLeft, QSize(d->p->width(), d->p->height()));
//         pixRect.translate(10, 4);
        painter->drawPixmap(pixRect, temp); //*d->p);
//         painter->restore();

    } else {
//         if (d->timeLine->state() == QTimeLine::NotRunning) {
//             d->index = QModelIndex();
//         }
    }

    if (d->optV4.state & QStyle::State_Selected) {
//         painter->save();
//         QRect pixRect(d->optV4.rect.topLeft(), QSize(d->p->width(), d->p->height()));
        QPoint topLeft(d->optV4.rect.x()+((d->optV4.decorationSize.width()-d->p->width())/2),
                       d->optV4.rect.y()+((d->optV4.decorationSize.height()-d->p->height())/2));
        QRect pixRect(topLeft, QSize(d->p->width(), d->p->height()));
//         pixRect.translate(4, 10);
        painter->drawPixmap(pixRect, *d->p);
//         painter->restore();
    }

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

//     QStyledItemDelegate::paint(painter, d->optV4, index);

    //here follows the implementation of the painting process
    //instead of using the one from QStyledItemDelegate.
    //this will ease some deep changes in the paint process
    //when needed.

    painter->save();
//     painter->setClipRect(d->optV4.rect);

    QPixmap pixmapDecoration = d->optV4.icon.pixmap(QSize(iconSize, iconSize));

    QRect decorationRect = d->optV4.rect;
    decorationRect.translate( (decorationRect.width() - pixmapDecoration.width() ) / 2,
                              (decorationRect.height() - pixmapDecoration.height()) / 2);

    decorationRect.setSize(QSize(pixmapDecoration.width(), pixmapDecoration.height()));
    painter->drawPixmap(decorationRect, pixmapDecoration);

    painter->setPen(d->optV4.palette.color(QPalette::Text));

    // FIXME store the QString instead of calling index.data() many times
    QRect textRect = d->optV4.rect;
    textRect.translate( 0, decorationRect.height()+pixmapDecoration.height()+textMargin);
    textRect.setSize(QSize(textRect.width(), textRect.height() - decorationRect.height()));
    painter->drawText(textRect, Qt::AlignHCenter, index.data().toString());

    painter->restore();
}

void RaptorItemDelegate::drawSingleAppWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
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

    //TODO: use standard delegate roles
    painter->drawText(descriptionRect, Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    painter->restore();
}

void RaptorItemDelegate::generateBgPixmap(const QSize &s) const // TODO find a way to make this themable, preferrably via SVG.
{
    if (!d->p) { // it's an expensive operation, so let's keep a cached pixmap.
        qreal blurAmount = 7;
        QSize rectSize(s.width()-(blurAmount*3), s.height()-(blurAmount*3));
        QImage *i = new QImage(d->optV4.decorationSize, QImage::Format_ARGB32_Premultiplied);
        i->fill(0);
        QPainter p(i);
        QLinearGradient lg(0, blurAmount*2, 0, d->optV4.decorationSize.height()-blurAmount);
        QColor sel = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
        lg.setColorAt(0.0, sel);
        QColor sel2 = sel;
        sel2.setAlpha(40);
        lg.setColorAt(1.0, sel2);
        p.setBrush(lg);
        p.setPen(Qt::NoPen);
        p.drawRect(blurAmount, blurAmount, rectSize.width(), rectSize.height());

        p.end();
        
        expblur<16, 7>(*i, blurAmount);

        d->p = new QPixmap(d->optV4.decorationSize);
        d->p->fill(Qt::transparent);
        
        QPainter pp(d->p);
        pp.setCompositionMode(QPainter::CompositionMode_Source);
        pp.drawImage(0, 0, *i);
        pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        pp.fillRect(d->p->rect(), QColor(0, 0, 0, 140));
        pp.end();

        delete i;
    }
}

void RaptorItemDelegate::animatePaint(int frame)
{
    d->frame = frame;

    if (!d->view) { // TODO: we should avoid the use of d->view when fully ported to QGW
        return;
    }

    d->view->update(d->optV4.rect);
}

void RaptorItemDelegate::setTextColor(const QColor &color)
{
    d->textColor = color;
}

void RaptorItemDelegate::setViewMode(RaptorItemDelegate::ViewMode mode)
{
    d->mode = mode;
}

RaptorItemDelegate::ViewMode RaptorItemDelegate::viewMode()
{
    return d->mode;
}

