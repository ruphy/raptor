/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

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

#include <KDebug>

Breadcrumb::Breadcrumb(QAbstractItemModel *model, QGraphicsWidget *parent) : QGraphicsWidget(parent), m_model(model)
{
    setCurrentItem(QModelIndex());
}

Breadcrumb::~Breadcrumb()
{}

void Breadcrumb::setCurrentItem(const QModelIndex &index)
{
    // maybe this is a bit drastic
    qDeleteAll(m_items);
    m_items.clear();

    if (!index.isValid()) { // we always set the main menu as first element
        m_items << new BreadcrumbItem; // the arrow
        BreadcrumbItem *mainMenu = new BreadcrumbItem;
        mainMenu->setIsMainMenu(true);
        m_items << mainMenu;
        return;
    }

    QModelIndex currentIndex = index;

    qreal x = 0;
    do {
        BreadcrumbItem *item = new BreadcrumbItem(currentIndex);
        item->setRect(QRectF(x, 0, contentsRect().height(), contentsRect().height()));
        m_items.prepend(item);
        m_items.prepend(new BreadcrumbItem); // the arrow

        x += contentsRect().height();
        currentIndex = m_model->parent(currentIndex);
    } while (currentIndex.isValid());

    BreadcrumbItem *mainMenu = new BreadcrumbItem;
    mainMenu->setIsMainMenu(true);
    m_items.prepend(mainMenu);
    m_items.prepend(new BreadcrumbItem);

    update();
}

void Breadcrumb::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    foreach (BreadcrumbItem *item, m_items) {
        item->icon().paint(painter, item->rect().toRect());
//         kDebug() << item->rect();
//         painter->fillRect(item->rect(), Qt::green);
    }
}

void Breadcrumb::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_UNUSED(event)

    qreal x = 0;
    foreach (BreadcrumbItem *item, m_items) {
        item->setRect(QRectF(x, 0, contentsRect().height(), contentsRect().height()));
        x += contentsRect().height();
    }

    update();
}

