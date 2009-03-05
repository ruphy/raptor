/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef BREADCRUMB_H
#define BREADCRUMB_H

#include <QGraphicsWidget>
#include <QList>

class BreadcrumbItem;
class QModelIndex;
class QAbstractItemModel;
class QGraphicsSceneResizeEvent;
class QGraphicsSceneMouseEvent;
class QRectF;
class QTimeLine;
class RaptorGraphicsView;

class Breadcrumb : public QGraphicsWidget
{
    Q_OBJECT
public:
    Breadcrumb(RaptorGraphicsView *view, QGraphicsWidget *parent = 0);
    ~Breadcrumb();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void updateItemRects();

public slots:
    void setCurrentItem(const QModelIndex &index);

signals:
    /**
     * This signal is emitted when the user clicks an
     * item of the breadcrumb.
     */
    void changedRootIndex(const QModelIndex &);

    void rootMenuRequested();

private:
    RaptorGraphicsView *m_view;
    QList<BreadcrumbItem*> m_items;
    BreadcrumbItem *m_currentShowing;
    QTimeLine *m_timeLine;
    qreal m_realFrame;

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

protected slots:
    void animate();
};

#endif
