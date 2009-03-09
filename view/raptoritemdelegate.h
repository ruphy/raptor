/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2008 Riccardo Iaconelli <riccardo@kde.org>
   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef RAPTORITEMDELEGATE_H
#define RAPTORITEMDELEGATE_H

#include "raptorgraphicsview.h"

#include <QStyledItemDelegate>

class QSize;
class QGraphicsWidget;

class RaptorItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum ViewMode {
        SingleApp = 0, //http://nuno-icons.com/images/estilo/raptor/oneapp.png
        TwoApps   = 1, //http://nuno-icons.com/images/estilo/raptor/menu2enteries.png
        Normal    = 2, //http://nuno-icons.com/images/estilo/raptor/main menu.png
        Search    = 4 //http://nuno-icons.com/images/estilo/raptor/find.png
    };

    enum DataRole {
        Description = Qt::UserRole + 1
    };

    RaptorItemDelegate(RaptorGraphicsView *parent);
    ~RaptorItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

    void setTextColor(const QColor &);

    void setViewMode(ViewMode);
    ViewMode viewMode();

signals:
    void favoriteAddRequested(const QModelIndex &);

protected:
    void drawNormalWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex &index) const;
    void drawTwoAppsWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    void drawSingleAppWay(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex &index) const;

    void drawFavIcon(QPainter *painter, const QRect &) const;
    void drawOverlay(QPainter *painter, const QRect &rect) const;

private:
    void generateBgPixmap(const QSize &s) const;
    class Private;
    Private *d;
};

#endif
