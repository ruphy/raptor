/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/ 

#ifndef RAPTORITEMSVIEW_H
#define RAPTORITEMSVIEW_H

#include <QListView>

class KUrl;

namespace Kickoff{
class ApplicationModel;
}

/**
 * A simple listview with smooth scrolling as base implementation for
 * the main items view of the menu.
 */

class RaptorItemsView : public QListView
{
    Q_OBJECT
    public:
        RaptorItemsView(QWidget *parent = 0);
        ~RaptorItemsView();

        void setOrientation(Qt::Orientation);
        void setModel(Kickoff::ApplicationModel *model);
        Kickoff::ApplicationModel* model();

    protected:
        int getNewScrollValue(const QRect &rect);
        void resizeEvent(QResizeEvent *event);

    public slots:
        /**
         * Scrolls smoothly till the element gets centered.
         */
        void smoothScrollTo(const QModelIndex &index);

        /**
         * Shows the entries of the clicked cathegory or
         * executes the relative application.
         */
        void enterItem(const QModelIndex &index);

        /**
         * Browse the menu back.
         */
        void browseBack();

    signals:
        /**
         * This signal is emitted when the user activates an item that is an application
         * @arg the .desktop url of the application.
         *
         * In order to launch the app you can do something like:
         *
         * KDesktopFile desktopFile(url.pathOrUrl());
         * KService service(&desktopFile);
         * KRun::run(service, KUrl::List(), yourwidget);
         */
        void applicationClicked(const KUrl &url);

    private:
        class RaptorItemsViewPrivate;
        RaptorItemsViewPrivate *d;
};
#endif
