/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "breadcrumbitem.h"
#include "breadcrumb.h"

#include <QModelIndex>
#include <QRectF>
#include <QFontMetrics>
#include <QTimeLine>
#include <QPainter>

#include <KIcon>
#include <KLocale>
#include <KGlobalSettings>

#include <Plasma/Theme>
#include <Plasma/Svg>

BreadcrumbItem::BreadcrumbItem(const QModelIndex &index) : m_arrow(false), m_mainMenu(false), m_textWidth(-1), m_showingText(false)
{
    if (!index.isValid()) {
        m_arrow = true;
        m_svg = new Plasma::Svg;
        m_svg->setImagePath("widgets/raptorarrows");
        m_svg->setContainsMultipleImages(true);
        return;
    }

    m_index = index;
}

BreadcrumbItem::~BreadcrumbItem()
{
    delete m_svg;
}

QString BreadcrumbItem::name() const
{
    if (m_arrow) {
        return QString();
    }

    if (m_mainMenu) {
        return i18n("Main Menu");
    }

    return m_index.data().toString();
}

QPixmap BreadcrumbItem::icon(int size) const
{
    if (m_arrow) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);

        m_svg->resize(size, size);

        QPainter p(&pixmap);
        m_svg->paint(&p, size / 4, size / 4, "rightarrow");
        p.end();

        return pixmap;
    }
    if (m_mainMenu) {
        return KIcon("go-home").pixmap(size, size);
    }

    return m_index.data(Qt::DecorationRole).value<QIcon>().pixmap(size, size);
}

void BreadcrumbItem::setRect(const QRectF &rect)
{
    m_rect = rect;
}

QRectF BreadcrumbItem::rect() const
{
    return m_rect;
}

bool BreadcrumbItem::isArrow()
{
    return m_arrow;
}

void BreadcrumbItem::setIsMainMenu(bool set)
{
    m_mainMenu = set;
    m_arrow = !set;
}

bool BreadcrumbItem::isMainMenu()
{
    return m_mainMenu;
}

QModelIndex BreadcrumbItem::index() const
{
    return m_index;
}

int BreadcrumbItem::textWidth()
{
    if (m_textWidth == -1) {
        m_textWidth = Plasma::Theme::defaultTheme()->fontMetrics().width(name());
    }
    return m_textWidth;
}

bool BreadcrumbItem::showingText()
{
     return m_showingText;
}

void BreadcrumbItem::setShowingText(bool set)
{
    m_showingText = set;
}

void BreadcrumbItem::setTextRect(const QRectF &rect)
{
    m_textRect = rect;
}

QRectF BreadcrumbItem::textRect() const
{
    return m_textRect;
}
