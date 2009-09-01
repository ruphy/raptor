/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2008 Riccardo Iaconelli <riccardo@kde.org>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "raptoritemdelegate.h"
#include "engine/kickoff/favoritesmodel.h"
#include "engine/nepomuk/nepomukmodel.h"

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
/*const int ANIMATION_DURATION = 200; // will need to be made shorter once we have keyboard navigation.*/
const int FAV_ICON_SIZE = 22;
const int AGO_WIDTH = Plasma::Theme::defaultTheme()->fontMetrics().width(i18n("ago"));
const int USED_WIDTH = Plasma::Theme::defaultTheme()->fontMetrics().width(i18n("Used"));
const int SPACE_WIDTH = Plasma::Theme::defaultTheme()->fontMetrics().width(" ");
const int TEXT_HEIGHT = Plasma::Theme::defaultTheme()->fontMetrics().height();

class RaptorItemDelegate::Private
{
    public:
        Private(RaptorGraphicsView *view, RaptorItemDelegate *q):
                q(q),
                view(view),
                textColor(QColor()),
                p(0),
                svg(0),
                mode(RaptorItemDelegate::Normal),
                item(0)
                {
                    svg = new Plasma::Svg(q);
                    svg->setImagePath("widgets/overlay");
                    svg->setContainsMultipleImages(true);
                }

        ~Private()
                { delete p; delete svg;}

    RaptorItemDelegate *q;

    QStyleOptionViewItemV4 optV4;
    RaptorGraphicsView *view;

    QColor textColor;
    QPixmap *p;

    Plasma::Svg *svg;

    RaptorItemDelegate::ViewMode mode;

    QRect favIconRect;

    RaptorMenuItem *item;
};

RaptorItemDelegate::RaptorItemDelegate(RaptorGraphicsView *parent) : QStyledItemDelegate(parent),
                                                          d(new Private(parent, this))
{
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

    foreach (RaptorMenuItem *current, d->view->shownItems()) {
        if (current->modelIndex() == index) {
            d->item = current;
        }
    }

    switch (d->mode) {
        case RaptorItemDelegate::Normal:
            drawNormalWay(painter, option, index);
            break;
        case RaptorItemDelegate::SingleApp:
            drawSingleAppWay(painter, option, index);
            break;
        case RaptorItemDelegate::TwoApps:
            drawTwoAppsWay(painter, option, index);
            break;
        default:
            drawNormalWay(painter, option, index);
            break;
    }
}

void RaptorItemDelegate::drawNormalWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const qreal textMargin = 3;
    const qreal iconSize = option.rect.height() - textMargin - TEXT_HEIGHT;

    d->optV4 = option;
    initStyleOption(&d->optV4, index);
    
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        painter->save();
        QRect overlayRect = d->optV4.rect;
        overlayRect.setSize(QSize(overlayRect.height(), overlayRect.height()));
        overlayRect.moveCenter(d->optV4.rect.center());
        painter->setOpacity(d->item->timeLine()->currentValue());
        drawOverlay(painter, overlayRect);
        painter->restore();
    }

    painter->save();

    QPixmap pixmapDecoration = d->optV4.icon.pixmap(QSize(iconSize / 1.4, iconSize / 1.4));//FIXME: 1.4 is a magic number so make it static int ;)

    QRect decorationRect = d->optV4.rect;
    decorationRect.translate( (decorationRect.width() - pixmapDecoration.width() ) / 2,
                              (decorationRect.height() - pixmapDecoration.height()) / 2);

    decorationRect.setSize(QSize(pixmapDecoration.width(), pixmapDecoration.height()));
//     painter->fillRect(decorationRect, Qt::green);
    painter->drawPixmap(decorationRect, pixmapDecoration);

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        drawFavIcon(painter, decorationRect, index);
    }

    painter->setPen(d->optV4.palette.color(QPalette::Text));

    // FIXME store the QString instead of calling index.data() many times
    QRect textRect = d->optV4.rect;
    textRect.setY(decorationRect.y() + decorationRect.height() + textMargin);
    painter->drawText(textRect, Qt::AlignHCenter, index.data().toString());

    painter->restore();
}

void RaptorItemDelegate::drawTwoAppsWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const qreal textMargin = 3;
    const qreal iconSize = option.rect.height() - textMargin - TEXT_HEIGHT;

    d->optV4 = option;
    initStyleOption(&d->optV4, index);

    if (d->textColor != QColor()) {
        d->optV4.palette.setColor(QPalette::Text, d->textColor);
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setClipRect(d->optV4.rect);
    painter->setPen(Qt::NoPen);

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        QRect overlayRect = d->optV4.rect;
        overlayRect.setWidth(d->optV4.rect.height());
        painter->save();
        painter->setOpacity(d->item->timeLine()->currentValue());
        drawOverlay(painter, overlayRect);
        painter->restore();
    }

    painter->save();

    QPixmap pixmapDecoration = d->optV4.icon.pixmap(QSize(iconSize / 1.4, iconSize / 1.4));//FIXME: 1.4 is a magic number so make it static int ;)

    QRect decorationRect = d->optV4.rect;

    decorationRect.setSize(QSize(pixmapDecoration.width(), pixmapDecoration.height()));
    decorationRect.moveCenter(QRect(d->optV4.rect.x(), d->optV4.rect.y(), d->optV4.rect.height(), d->optV4.rect.height()).center());

    //decorationRect.translate( (decorationRect.width() - pixmapDecoration.width() ) / 2,
    //                          (decorationRect.height() - pixmapDecoration.height()) / 2);

//     painter->fillRect(decorationRect, Qt::green);

    painter->drawPixmap(decorationRect, pixmapDecoration);

    if (d->optV4.state & QStyle::State_MouseOver && !(d->optV4.state & QStyle::State_Selected) ) {
        drawFavIcon(painter, decorationRect, index);
    }

    painter->setPen(d->optV4.palette.color(QPalette::Text));

    // FIXME store the QString instead of calling index.data() many times
    QRect textRect = d->optV4.rect;
    textRect.setY(decorationRect.y() + decorationRect.height() + textMargin);
    textRect.setWidth(decorationRect.width());
    textRect.moveCenter(QPoint(d->optV4.rect.x() + d->optV4.rect.height() / 2, textRect.y() + textRect.height() / 2));
    painter->drawText(textRect, Qt::AlignHCenter, index.data().toString());

    QRect usedRect = d->optV4.rect;
    usedRect.setSize(QSize(USED_WIDTH, TEXT_HEIGHT));
    usedRect.translate(d->optV4.rect.width() - USED_WIDTH - textMargin, d->optV4.rect.height() / 2 - TEXT_HEIGHT - textMargin / 2);
    painter->drawText(usedRect, Qt::AlignRight, i18n("Used"));

    QRect lastUsedRect = d->optV4.rect;
    lastUsedRect.setSize(QSize(d->item->lastUsedWidth() + SPACE_WIDTH + AGO_WIDTH, TEXT_HEIGHT));
    lastUsedRect.translate(d->optV4.rect.width() - lastUsedRect.width() - textMargin, usedRect.y() + textMargin + TEXT_HEIGHT);

    //kDebug() << lastUsedRect << item->lastUsed() << item->lastUsedWidth();

    painter->drawText(lastUsedRect, Qt::AlignRight, i18n("%1 ago", d->item->lastUsed()));

    //lastUsedRect.setY(lastUsedRect.y() + TEXT_HEIGHT());

    painter->restore();

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

    QPixmap pixmapDecoration = d->optV4.icon.pixmap(d->optV4.rect.height());

    QRect decorationRect = d->optV4.rect;
    decorationRect.setSize(QSize(d->optV4.rect.height(), d->optV4.rect.height()));

    painter->drawPixmap(decorationRect, pixmapDecoration);

    painter->setPen(d->optV4.palette.color(QPalette::Text));

    QRect descriptionRect = d->optV4.rect;
    descriptionRect.translate(decorationRect.width(), 0);
    descriptionRect.setSize(QSize(d->optV4.rect.width() - decorationRect.width(), descriptionRect.height()));
    descriptionRect.setY((d->optV4.rect.height() - painter->boundingRect(descriptionRect, Qt::AlignLeft, index.data(Qt::DisplayRole).toString()).height()) / 2);

    //TODO: use standard delegate roles
    painter->drawText(descriptionRect, Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    QRect lastUsedRect = d->optV4.rect;
    lastUsedRect.setSize(QSize(d->item->lastUsedWidth() + USED_WIDTH + SPACE_WIDTH * 2 + AGO_WIDTH, TEXT_HEIGHT));
    lastUsedRect.translate(d->optV4.rect.width() - lastUsedRect.width(), d->optV4.rect.height() - lastUsedRect.height());

    kDebug() << lastUsedRect << d->item->lastUsed() << d->item->lastUsedWidth();

    painter->drawText(lastUsedRect, Qt::AlignRight, i18n("Used %1 ago", d->item->lastUsed()));

    painter->restore();
}

bool RaptorItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model)
    Q_UNUSED(option)
    Q_UNUSED(index)

    if (event->type() != QEvent::GraphicsSceneMouseRelease) {
        return false;
    }

    QGraphicsSceneMouseEvent *clickEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
    if (d->favIconRect.contains(clickEvent->pos().toPoint())) {
        //kDebug() << "star clicked";
        emit favoriteAddRequested(index);
        return true;
    }

    return false;
}

void RaptorItemDelegate::drawFavIcon(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    if (!index.data(Qt::UserRole + 2).isNull()) { // we check whether it is an app or not
        painter->save();

        if (Kickoff::FavoritesModel::isFavorite(index.data(Qt::UserRole + 2).toString())) {
            painter->setOpacity(d->item->timeLine()->currentValue());
        } else {
            painter->setOpacity(d->item->timeLine()->currentValue() < 0.5 ? d->item->timeLine()->currentValue() : 0.5);
        }

        KIcon icon("favorites");
        QRect favRect = rect;
        favRect.setSize(QSize(FAV_ICON_SIZE, FAV_ICON_SIZE));
        favRect.translate(rect.width() - FAV_ICON_SIZE, 0);

        d->favIconRect = favRect;

        icon.paint(painter, favRect);

        painter->restore();
    }
}

void RaptorItemDelegate::drawOverlay(QPainter *painter, const QRect &rect) const
{
    generateBgPixmap(rect.size());

    painter->drawPixmap(rect, *d->p);
}

void RaptorItemDelegate::generateBgPixmap(const QSize &s) const // TODO find a way to make this themable, preferrably via SVG.
{
    if (d->p && d->p->size() != s) {
        delete d->p;
        d->p = 0;
    }

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

