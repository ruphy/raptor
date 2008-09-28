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

class RaptorDescriptionDelegate;

/**
 * A simple listview with smooth scrolling as base implementation for
 * the main items view of the menu.
 */

class RaptorItemsView : public QListView
{
    Q_OBJECT
    public:
        enum Mode {NormalMode, DescriptionMode};

        RaptorItemsView(QWidget *parent = 0);
        ~RaptorItemsView();

        void setOrientation(Qt::Orientation);
        void setModel(QAbstractItemModel *model);
        Kickoff::ApplicationModel* model();
        void hideScrollBars();
        void showScrollBars();
        void focusCentralItem();
        void setDescriptionDelegate(RaptorDescriptionDelegate *delegate);

        /**
         * This method has no effect if setDescriptionDelegate isn't called before
         */
        void switchMode(RaptorItemsView::Mode);

        //TODO: add setItemDelegate(RaptorItemDelegate *delegate); so that the view only uses it
        //TODO: add docs also for public members

    protected:
        int getNewScrollValue(const QRect &rect);
        void resizeEvent(QResizeEvent *event);
        QRect visualRect(const QModelIndex &index) const;

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
         * Browse the view back.
         */
        void browseBack();

        /**
         * Reimplemented from QAbstractItemView to smoothScroll to the item
         */
        void setCurrentIndex(const QModelIndex &index);

    private slots:
        void storeIndex();
        void restoreIndex();

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

        /**
         * This signal is emitted every time an item becomes the root item of
         * the view. Use this signal instead of clicked() when tracking view
         * changes.
         *
         * @arg entered is the just become root item.
         */
        void enteredItem(const QModelIndex &entered);

    private:
        class RaptorItemsViewPrivate;
        RaptorItemsViewPrivate *d;
};
#endif
