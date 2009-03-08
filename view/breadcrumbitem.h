/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef BREADCRUMBITEM_H
#define BREADCRUMBITEM_H

#include <QModelIndex>
#include <QRectF>

class Breadcrumb;
class QString;
class QTimeLine;

namespace Plasma {
    class Svg;
};

class BreadcrumbItem
{
public:
    /**
     * Passing no index will make the item be an arrow.
     */
    BreadcrumbItem(const QModelIndex &index = QModelIndex());
    ~BreadcrumbItem();

    bool isArrow();

    bool isMainMenu();
    void setIsMainMenu(bool set);

    /**
     * @Returns the icon of the item or an arrow icon if the
     * item is an arrow.
     */
    QPixmap icon(int size) const;

    QString name() const;

    void setRect(const QRectF &);
    QRectF rect() const;

    void setTextRect(const QRectF &);
    QRectF textRect() const;

    bool showingText();
    void setShowingText(bool);

    int textWidth();

    QModelIndex index() const;

private:
    QModelIndex m_index;
    bool m_arrow;
    bool m_mainMenu;

    QRectF m_rect;
    QRectF m_textRect;

    int m_textWidth;
    bool m_showingText;

    Plasma::Svg *m_svg;
};

#endif

