/* This file is part of the KDE project

   Copyright (C) 2009 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORGRAPHICSLAYOUT_H
#define RAPTORGRAPHICSLAYOUT_H

#include <QObject>
#include <QList>
class RaptorGraphicsView;
class RaptorMenuItem;
class QEvent;

class RaptorGraphicsLayout : public QObject
{
    Q_OBJECT
public:
    RaptorGraphicsLayout(RaptorGraphicsView *parent);
    ~RaptorGraphicsLayout();

    void setMenuItems(const QList<RaptorMenuItem*> &items);
    QList<RaptorMenuItem*> visibleItems() const;

    void setContentMargins(qreal left, qreal top, qreal right, qreal bottom);
    void invalidate();

    bool eventFilter(QObject *, QEvent *);

private:
    class Private;
    Private *d;
};

#endif
