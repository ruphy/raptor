/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORGRAPHICSVIEW_H
#define RAPTORGRAPHICSVIEW_H

#include "raptormenuitem.h"

#include <QGraphicsWidget>
#include <QModelIndex>

#include <KUrl>

class QAbstractItemModel;
class QGraphicsSceneResizeEvent;
class QGraphicsSceneHoverEvent;

class RaptorGraphicsView : public QGraphicsWidget
{
    Q_OBJECT
public:
    enum ViewMode //FIXME: Feel free to change the names here...
    {
        SingleApp = 0, //http://nuno-icons.com/images/estilo/raptor/oneapp.png
        TwoApps   = 1, //http://nuno-icons.com/images/estilo/raptor/menu2enteries.png
        Normal    = 2, //http://nuno-icons.com/images/estilo/raptor/main menu.png
        Search    = 4 //http://nuno-icons.com/images/estilo/raptor/find.png
    };
    
    RaptorGraphicsView(QGraphicsItem *parent = 0);
    ~RaptorGraphicsView();

    QModelIndex rootIndex() const;

    QList<RaptorMenuItem*> items() const;
    QList<RaptorMenuItem*> shownItems() const;

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel* model();
    
    void setViewMode(ViewMode mode);
    ViewMode viewMode();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
    void reset();
    void setRootIndex(const QModelIndex &);
    void scrollRight();
    void scrollLeft();

protected slots:
    void slotAddFavorite(const QModelIndex &);
    void addRows(const QModelIndex &, int, int);
    void removeRows(const QModelIndex &, int, int);

signals:
    void enteredItem(const QModelIndex &);
    void applicationClicked(const QModelIndex &);
    void favoriteClicked(const QString &);

protected:
    void getItems();
    void scrollItems();

//     void resizeEvent(QGraphicsSceneResizeEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    ViewMode viewModeFromItemCount();

private:
    class Private;
    Private *d;
};

#endif
