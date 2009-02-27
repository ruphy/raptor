/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORMENUITEM_H
#define RAPTORMENUITEM_H

#include <QObject>
#include <QRectF>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
class RaptorItemDelegate;

class RaptorMenuItem : public QObject
{
    Q_OBJECT
public:
    RaptorMenuItem(const QModelIndex &index, QObject *parent = 0);
    ~RaptorMenuItem();

    /**
     * Call this method in order to set the geometry of the item.
     * This setting will override the rect passed with
     * option in the paint() method.
     */
    void setRect(const QRectF &);

    /**
     * @returns the rect used to paint the item. If no
     * setRect has been called previously it will return the rect
     * used to paint the item with the first paint() call.
     * If no paint() occurred this method will return QRect().
     */
    QRectF rect() const;

    /**
     * This is a convenience function and it is the same as calling:
     * setRect ( rect().translated(dx, dy) );
     * A paint() call is needed after this call in order to apply
     * the geometry modification.
     */
    void moveBy(float dx, float dy);

    /**
     * @Returns the index associated to this item.
     */
    QModelIndex modelIndex() const;

private:
    class Private;
    Private *d;
};

#endif
