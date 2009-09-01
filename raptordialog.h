/* This file is part of the KDE project

   Copyright (C) 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORDIALOG_H
#define RAPTORDIALOG_H

#include <Plasma/Dialog>

class RaptorDialog : public Plasma::Dialog
{
    Q_OBJECT
    public:
        RaptorDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Window);
        ~RaptorDialog();

    protected:
        virtual void paintEvent(QPaintEvent *e);
};

#endif
