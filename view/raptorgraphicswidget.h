/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef RAPTORGRAPHICSWIDGET_H
#define RAPTORGRAPHICSWIDGET_H

#include <QGraphicsWidget>
#include <QModelIndex>
#include <plasma/querymatch.h>

class KUrl;
class KConfigGroup;

class RaptorGraphicsWidget : public QGraphicsWidget
{
    Q_OBJECT
    public:
        RaptorGraphicsWidget(QGraphicsItem *parent, const KConfigGroup &appletconfig);
        ~RaptorGraphicsWidget();

    public slots:
        void scrollLeft();
        void scrollRight();

    protected slots:
        void setFavoritesModel();
        void addOrRemoveFavorite(const QString &);

        void updateColors();
        void launchApplication(const QModelIndex &);
        void refineModel();
        void matchesChanged(const QList<Plasma::QueryMatch>&);

    private:
        class Private;
        Private *d;

};

#endif
