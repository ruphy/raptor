/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORSCROLLBUTTON_H
#define RAPTORSCROLLBUTTON_H

#include <QPushButton>

class QPaintEvent;

class RaptorScrollButton : public QPushButton
{
    Q_OBJECT
    public:
        RaptorScrollButton(QWidget * parent = 0);
        ~RaptorScrollButton();

        void paintEvent(QPaintEvent * event);

    private:
        class Private;
        Private * d;
}; 

#endif
