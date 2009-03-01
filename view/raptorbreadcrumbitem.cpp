/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>
   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "raptorbreadcrumbitem.h"

#include <Plasma/Theme>

#include <QPainter>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QTimeLine>
#include <QEvent>
#include <QStyleOptionGraphicsItem>

#include <KIcon>
#include <KDebug>

const int PIXMAP_SIZE = 22;
const int MARGIN = 5;

class RaptorBreadCrumbItem::Private
{
public:
    Private(const QString &t, const QIcon &i, RaptorBreadCrumbItem *q)
      : q(q),
        timeLine(0),
        frame(0),
        sizeHint(QSize(PIXMAP_SIZE, PIXMAP_SIZE)),
        text(t),
        icon(i)
    {
        timeLine = new QTimeLine(250, q);
    }

    RaptorBreadCrumbItem *q;
    QModelIndex index;
    QTimeLine * timeLine;
    int frame;
    QColor textColor;
    int fontWidth;
    QSize sizeHint;
    QRect textRect;
    QString text;
    QIcon icon;
};

RaptorBreadCrumbItem::RaptorBreadCrumbItem(const QIcon & icon, const QString & text,
                                           const QModelIndex &index, QGraphicsWidget * parent)
  : QGraphicsWidget(parent),
    d(new Private(text, icon, this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAcceptHoverEvents(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    d->index = index;

    updateColors();

    installEventFilter(this);

    connect(d->timeLine, SIGNAL(frameChanged(int)), SLOT(animatePaint(int)));
    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), SLOT(updateColors()));
}

RaptorBreadCrumbItem::~RaptorBreadCrumbItem()
{
    delete d;
}

void RaptorBreadCrumbItem::paint(QPainter * p, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (d->frame) {
        //FIXME: avoid magic numbers
        p->setPen(d->textColor);
        p->drawText(d->textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
        QRect pixmapRect(option->rect);
        pixmapRect.setSize(QSize(PIXMAP_SIZE, PIXMAP_SIZE));
        pixmapRect.translate(d->textRect.width() + MARGIN, 0);
        p->drawPixmap(pixmapRect, icon().pixmap(PIXMAP_SIZE, PIXMAP_SIZE));
    }
    else {
        p->drawPixmap(option->rect, icon().pixmap(PIXMAP_SIZE, PIXMAP_SIZE));
    }

    p->end();
}

const QModelIndex RaptorBreadCrumbItem::index()
{
    return d->index;
}

QString RaptorBreadCrumbItem::text() const
{
    return d->text;
}

QIcon RaptorBreadCrumbItem::icon() const
{
    return d->icon;
}

void RaptorBreadCrumbItem::emitNavigationRequested()
{
    emit navigationRequested(index(), this);
}

bool RaptorBreadCrumbItem::eventFilter(QObject * watched, QEvent * event)
{
    Q_UNUSED(watched)
    switch(event->type())
    {
        case QEvent::GraphicsSceneHoverEnter:
            d->timeLine->setFrameRange(0, 20);
            d->timeLine->start();
            return false;
        case QEvent::GraphicsSceneHoverLeave:
            d->timeLine->stop();
            d->timeLine->setFrameRange(20, 0);
            d->timeLine->start();
//             d->frame = 0;
//             updateSizes();
//             repaint();
            return false;
        case QEvent::GraphicsSceneMousePress:
            emitNavigationRequested();
            return false;
        default:
            return false;
    };
}

void RaptorBreadCrumbItem::animatePaint(int frame)
{
    d->frame = frame;
    d->textRect = QRect(contentsRect().x(), contentsRect().y(), ((d->frame * d->fontWidth) / 20 ), PIXMAP_SIZE);

    updateSizes();
    update();
}

void RaptorBreadCrumbItem::updateSizes()
{
    if (d->frame) {
        d->sizeHint = QSize(((d->frame * d->fontWidth) / 20 ) + MARGIN + PIXMAP_SIZE, PIXMAP_SIZE);
    } else {
        d->sizeHint = QSize(PIXMAP_SIZE, PIXMAP_SIZE);
    }

    setMinimumSize(d->sizeHint);
    resize(d->sizeHint);
    updateGeometry();
}

void RaptorBreadCrumbItem::updateColors()
{
    d->textColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
    d->fontWidth = Plasma::Theme::defaultTheme()->fontMetrics().width(text());
}

QSizeF RaptorBreadCrumbItem::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const
{
    Q_UNUSED(which);
    Q_UNUSED(constraint);
    return d->sizeHint;
}

class RaptorBreadCrumbArrow::Private
{
    public:
        Private(QAbstractItemModel * m)
          : model(m)
        {}
        ~Private() {}

        QAbstractItemModel * model;
};

RaptorBreadCrumbArrow::RaptorBreadCrumbArrow(const QModelIndex &index, QAbstractItemModel * model, QGraphicsWidget * parent)
  : RaptorBreadCrumbItem(KIcon("arrow-right"), QString(), index, parent),
    d(new Private(model))
{
    removeEventFilter(this);
}

RaptorBreadCrumbArrow::~RaptorBreadCrumbArrow()
{
    delete d;
}

void RaptorBreadCrumbArrow::paint(QPainter * p, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->drawPixmap(option->rect, icon().pixmap(22, 22));
    p->end();
}

#include "raptorbreadcrumbitem.moc"
