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
#include <Plasma/Svg>

#include <KDebug>
#include <KIcon>

#include "blur.cpp" //TODO: make this a function in Plasma::PaintUtils

// FIXME: use Animator, for a shared timer.
const int ANIMATION_DURATION = 200; // will need to be made shorter once we have keyboard navigation.
const int FRAMES = 50;
const int FAV_ICON_SIZE = 22;

class RaptorItemDelegate::Private
{
    public:
        Private(QGraphicsWidget *view, RaptorItemDelegate *q):
                q(q),
                view(view),
                timeLine(new QTimeLine(ANIMATION_DURATION, q)),
                frame(1.0),
                textColor(QColor()),
                p(0),
                svg(0),
                mode(RaptorItemDelegate::Normal)
                {
                    svg = new Plasma::Svg(q);
                    svg->setImagePath("widgets/overlay");
                    svg->setContainsMultipleImages(false);
                }

        ~Private()
                { delete p; delete timeLine; }

    RaptorItemDelegate *q;

    QStyleOptionViewItemV4 optV4;
    QGraphicsWidget *view;

    QTimeLine *timeLine;
    qreal frame;

    QModelIndex index;

    QColor textColor;
    QPixmap *p;

    Plasma::Svg *svg;

    RaptorItemDelegate::ViewMode mode;

    QModelIndex currentIndex;
};

RaptorItemDelegate::RaptorItemDelegate(QGraphicsWidget *parent) : QStyledItemDelegate(parent),
                                                          d(new Private(parent, this))
{
    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(animatePaint()));
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
    
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    //d->optV4.decorationSize = QSize(64, 64);

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {

        d->optV4.state &= ~QStyle::State_MouseOver; //this removes the mouseOver state in order to draw a nicer selection rect

        if (d->timeLine->state() == QTimeLine::NotRunning && d->currentIndex != index) {
            d->currentIndex = index;
            d->timeLine->setFrameRange(0, FRAMES);
            d->timeLine->start();
        }

        generateBgPixmap(d->optV4.rect.size());
        painter->save();
        painter->setOpacity(d->frame);
        painter->drawPixmap(d->optV4.rect, *d->p);

        drawFavIcon(painter, d->optV4.rect);

        painter->restore();
    }

    if (d->optV4.state & QStyle::State_Selected) {
        QPoint topLeft(d->optV4.rect.x()+((d->optV4.decorationSize.width()-d->p->width())/2),
                       d->optV4.rect.y()+((d->optV4.decorationSize.height()-d->p->height())/2));
        QRect pixRect(topLeft, QSize(d->p->width(), d->p->height()));
        painter->drawPixmap(pixRect, *d->p);
    }

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

    painter->save();

    QPixmap pixmapDecoration = d->optV4.icon.pixmap(QSize(iconSize, iconSize));

    QRect decorationRect = d->optV4.rect;
    decorationRect.translate( (decorationRect.width() - pixmapDecoration.width() ) / 2,
                              (decorationRect.height() - pixmapDecoration.height()) / 2);

    decorationRect.setSize(QSize(pixmapDecoration.width(), pixmapDecoration.height()));
//     painter->fillRect(decorationRect, Qt::green);
    painter->drawPixmap(decorationRect, pixmapDecoration);

    painter->setPen(d->optV4.palette.color(QPalette::Text));

    // FIXME store the QString instead of calling index.data() many times
    QRect textRect = d->optV4.rect;
    textRect.setY(decorationRect.y() + decorationRect.height() + textMargin);
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

void RaptorItemDelegate::drawFavIcon(QPainter *painter, const QRect &rect) const
{
    KIcon icon("favorites");
    QRect favRect(0, 0, FAV_ICON_SIZE, FAV_ICON_SIZE);
    favRect.translate(rect.x() + rect.width() - FAV_ICON_SIZE, 0);

    icon.paint(painter, favRect);
}

void RaptorItemDelegate::generateBgPixmap(const QSize &s) const // TODO find a way to make this themable, preferrably via SVG.
{
    if (!d->p) { // it's an expensive operation, so let's keep a cached pixmap.
        d->p = new QPixmap(s);
        d->p->fill(Qt::transparent);

        QPainter p(d->p);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.setRenderHint(QPainter::Antialiasing);

        d->svg->resize(s);
        d->svg->paint(&p, QRectF(QPointF(0.0, 0.0), s), "overlay");

        p.end();
    }
}

void RaptorItemDelegate::animatePaint()
{
    d->frame = d->timeLine->currentValue();

    if (!d->view) {
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

