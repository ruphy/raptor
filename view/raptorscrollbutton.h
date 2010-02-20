/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORSCROLLBUTTON_H
#define RAPTORSCROLLBUTTON_H

#include <QWidget>

class QPainter;
class QHoverEvent;

class RaptorScrollButton : public QWidget
{
    Q_OBJECT
    public:
        enum Side
        {
           Left,
           Right
        };
        RaptorScrollButton(Side side, QWidget * parent = 0);
        ~RaptorScrollButton();

        //void paint(QPainter * p, const QStyleOptionGraphicsItem * option, QWidget * widget);
        void paintEvent(QPaintEvent *event);
        bool eventFilter(QObject * watched, QEvent * event);

    signals:
        void clicked();

    private slots:
        void animatePaint(int frame);

    private:
        class Private;
        Private * d;
}; 

#endif
