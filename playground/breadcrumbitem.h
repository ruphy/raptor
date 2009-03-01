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

class BreadcrumbItem : public QObject
{
Q_OBJECT
public:
    /**
     * Passing no index will make the item be an arrow.
     */
    BreadcrumbItem(const QModelIndex &index = QModelIndex(), Breadcrumb * parent = 0);
    ~BreadcrumbItem();

    bool isArrow();

    bool isMainMenu();
    void setIsMainMenu(bool set);

    /**
     * @Returns the icon of the item or an arrow icon if the
     * item is an arrow.
     */
    QIcon icon() const;
    QString name() const;

    void setRect(const QRectF &);
    QRectF rect() const;
    bool showText();

    int textWidth();

    QModelIndex index() const;

    void animateShowing();
    void animateHiding();

private slots:
    void animate(int frame);

private:
    QModelIndex m_index;
    bool m_arrow;
    bool m_mainMenu;
    QRectF m_rect;
    int m_textWidth;
    bool m_showText;
    QTimeLine * m_timeLine;
    Breadcrumb * m_parent;
};

#endif

