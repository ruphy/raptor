/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "breadcrumb.h"
#include "breadcrumbitem.h"

#include <QRectF>
#include <QIcon>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <KDebug>

Breadcrumb::Breadcrumb(QAbstractItemModel *model, QGraphicsWidget *parent) : QGraphicsWidget(parent), m_model(model), m_current(0)
{
    setCurrentItem(QModelIndex());
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    installEventFilter(this);
}

Breadcrumb::~Breadcrumb()
{}

void Breadcrumb::setCurrentItem(const QModelIndex &index)
{
    // maybe this is a bit drastic
    qDeleteAll(m_items);
    m_items.clear();


    if (!index.isValid()) { // we always set the main menu as first element
        m_items << new BreadcrumbItem(QModelIndex(), this);; // the arrow
        BreadcrumbItem *mainMenu = new BreadcrumbItem(QModelIndex(), this);
        mainMenu->setIsMainMenu(true);
        m_items << mainMenu;

        updateItemRects();
        update();

        return;
    }

    QModelIndex currentIndex = index;

    do {
        BreadcrumbItem *item = new BreadcrumbItem(currentIndex, this);
        m_items.prepend(item);
        m_items.prepend(new BreadcrumbItem(QModelIndex(), this)); // the arrow

        currentIndex = m_model->parent(currentIndex);
    } while (currentIndex.isValid());

    BreadcrumbItem *mainMenu = new BreadcrumbItem(QModelIndex(), this);
    mainMenu->setIsMainMenu(true);
    m_items.prepend(mainMenu);
    m_items.prepend(new BreadcrumbItem(QModelIndex(), this));


    updateItemRects();
    update();
}

void Breadcrumb::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    foreach (BreadcrumbItem *item, m_items) {
        if (item->showText()) {
            painter->setPen(QColor(Qt::white));//FIXME: Use theme color
            QRect textRect(item->rect().toRect());
            textRect.setWidth(item->textWidth());
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, item->name());
            QRect pixmapRect(item->rect().toRect());
            pixmapRect.setSize(QSize(pixmapRect.height(), pixmapRect.height()));
            pixmapRect.translate(item->textWidth()/* + MARGIN*/, 0);
            painter->drawPixmap(pixmapRect, item->icon().pixmap(pixmapRect.height(), pixmapRect.height()));
        }
        else {
            item->icon().paint(painter, item->rect().toRect());
        }
    }
}

void Breadcrumb::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_UNUSED(event)

    updateItemRects();
    update();
}

void Breadcrumb::updateItemRects()
{
    qreal x = 0;
    foreach (BreadcrumbItem *item, m_items) {
        if (item->showText()) {
            item->setRect(QRectF(x, 0, contentsRect().height() + item->textWidth(), contentsRect().height()));
            x += contentsRect().height() + item->textWidth();
        }
        else {
            item->setRect(QRectF(x, 0, contentsRect().height(), contentsRect().height()));
            x += contentsRect().height();
        }
    }
}

bool Breadcrumb::eventFilter(QObject * watched, QEvent * event)
{
    BreadcrumbItem * current = 0;
    QGraphicsSceneMouseEvent * e = static_cast<QGraphicsSceneMouseEvent*>(event);
    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneHoverEnter:
        case QEvent::GraphicsSceneHoverLeave:
        case QEvent::GraphicsSceneHoverMove:
        {
            foreach (BreadcrumbItem *item, m_items) {
                if (item->rect().contains(e->pos())) {
                    current = item;
                    break;
                }
            }
            break;
        }
        default:
            break;
    }

    if (current) {
        switch (event->type()) {
            case QEvent::GraphicsSceneMousePress:
                emit changedRootIndex(current->index());
                break;
            case QEvent::GraphicsSceneHoverMove:
                if (m_current && (!m_current->rect().contains(e->pos()) || m_current != current)) {
                    m_current->animateHiding();
                    m_current = 0;
                }
            case QEvent::GraphicsSceneHoverEnter:
                if (!m_current || m_current != current) {
                    m_current = current;
                    m_current->animateShowing();
                }
                break;
            case QEvent::GraphicsSceneHoverLeave:
                if (m_current) {
                    m_current->animateHiding();
                }
                break;
            default:
                break;
        }
    }
    return false;
}
