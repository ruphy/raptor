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
#include "raptorgraphicsview.h"

#include <QRectF>
#include <QIcon>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTimeLine>

#include <KDebug>

#include <Plasma/Theme>

const int DURATION = 250; //ms
const int FRAMES = 20;

Breadcrumb::Breadcrumb(RaptorGraphicsView *view, QGraphicsWidget *parent) : QGraphicsWidget(parent),
                                                                             m_view(view),
                                                                             m_currentShowing(0),
                                                                             m_timeLine(new QTimeLine(DURATION, this)),
                                                                             m_realFrame(1.0)
{
    setCurrentItem(QModelIndex());
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);

    connect (m_timeLine, SIGNAL(frameChanged(int)), this, SLOT(animate()));
}

Breadcrumb::~Breadcrumb()
{}

void Breadcrumb::setCurrentItem(const QModelIndex &index)
{
    // maybe this is a bit drastic
    qDeleteAll(m_items);
    m_items.clear();


    if (!index.isValid()) { // we always set the main menu as first element
        m_items << new BreadcrumbItem(QModelIndex()); // the arrow
        BreadcrumbItem *mainMenu = new BreadcrumbItem(QModelIndex());
        mainMenu->setIsMainMenu(true);
        m_items << mainMenu;

        updateItemRects();
        update();

        return;
    }

    QModelIndex currentIndex = index;

    do {
        BreadcrumbItem *item = new BreadcrumbItem(currentIndex);
        m_items.prepend(item);
        m_items.prepend(new BreadcrumbItem(QModelIndex())); // the arrow

        currentIndex = m_view->model()->parent(currentIndex);
    } while (currentIndex.isValid());

    BreadcrumbItem *mainMenu = new BreadcrumbItem(QModelIndex());
    mainMenu->setIsMainMenu(true);
    m_items.prepend(mainMenu);
    m_items.prepend(new BreadcrumbItem(QModelIndex()));


    updateItemRects();
    update();
}

void Breadcrumb::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    foreach (BreadcrumbItem *item, m_items) {
        painter->drawPixmap(item->rect().toRect(), item->icon(item->rect().width()));
        painter->save();
        painter->setClipRect(item->textRect());
        painter->setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
        painter->drawText(item->textRect(), Qt::AlignVCenter, item->name());
        painter->restore();
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
            if (item->showingText()) {
                item->setTextRect(QRectF(x, 0, m_realFrame*item->textWidth(), contentsRect().height()));
            } else if (item->textRect().width()) {
                item->setTextRect(QRectF(x, 0, (1.0 - m_realFrame)*item->textWidth(), contentsRect().height()));
            }
            x += item->textRect().width();
            item->setRect(QRectF(x, 0, contentsRect().height(), contentsRect().height()));
            x += contentsRect().height();
    }
}

void Breadcrumb::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    foreach (BreadcrumbItem *item, m_items) {
        if (item->rect().contains(event->pos()) || item->textRect().contains(event->pos())) {
            if (item->isMainMenu()) {
                emit rootMenuRequested();
            } else {
                emit changedRootIndex(item->index());
            }
            return;
        }
    }
}

void Breadcrumb::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    foreach (BreadcrumbItem *item, m_items) {
        if (item->rect().contains(event->pos())) {
            if (m_currentShowing) {
                m_currentShowing->setShowingText(false);
            }
            m_currentShowing = item;
            item->setShowingText(true);

            if (m_timeLine->state() == QTimeLine::NotRunning) {
                m_timeLine->setFrameRange(0, FRAMES);
                m_timeLine->start();
            }
            break;
        }
    }
}

void Breadcrumb::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!m_currentShowing) {
        return;
    }

    if (m_currentShowing->showingText()) {
        m_currentShowing->setShowingText(false);
        m_currentShowing = 0;

        if (m_timeLine->state() == QTimeLine::NotRunning) {
            m_timeLine->setFrameRange(0, FRAMES);
            m_timeLine->start();
        }
    }
}

void Breadcrumb::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if ( m_currentShowing && (m_currentShowing->rect().contains(event->pos()) || m_currentShowing->textRect().contains(event->pos())) ) {
        return;
    }

    foreach (BreadcrumbItem *item, m_items) {
        if ( ( item->rect().contains(event->pos()) || item->textRect().contains(event->pos()) )) {
            if (m_currentShowing) {
                m_currentShowing->setShowingText(false);
            }
            item->setShowingText(true);
            m_currentShowing = item;

            if (m_timeLine->state() == QTimeLine::NotRunning) {
                m_timeLine->setFrameRange(0, FRAMES);
                m_timeLine->start();
            }
            return;
        }
    }

    // if we are here the mouse is over an empty space so let's close the open item
    if (!m_currentShowing) {
        return;
    }

    m_currentShowing->setShowingText(false);
    m_currentShowing = 0;

    if (m_timeLine->state() == QTimeLine::NotRunning) {
        m_timeLine->setFrameRange(0, FRAMES);
        m_timeLine->start();
    }

}

void Breadcrumb::animate()
{
    m_realFrame = m_timeLine->currentValue();
    updateItemRects();
    update();
}
