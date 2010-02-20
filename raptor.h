/* This file is part of the KDE project

   Copyright (C) 2008 - 2009 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
// Here we avoid loading the header multiple times
#ifndef RAPTOR_HEADER
#define RAPTOR_HEADER
// We need the Plasma Applet headers
#include <KIcon>

#include <Plasma/PopupApplet>
#include <Plasma/IconWidget>
#include <Plasma/Dialog>

class RaptorGraphicsWidget;
class RaptorScrollButton;

class QSizeF;

// Define our plasma Applet
class Raptor : public Plasma::PopupApplet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        Raptor(QObject *parent, const QVariantList &args);
        ~Raptor();

        void init();
        void constraintsEvent(Plasma::Constraints constraints);

        QGraphicsWidget *graphicsWidget();
        
    protected:
        virtual void popupEvent(bool shown);
	
    private:
        void updateScrollButtonPositions();

    private:
        RaptorGraphicsWidget *m_gwidget;
        RaptorScrollButton *m_rightScrollButton;
        RaptorScrollButton *m_leftScrollButton;
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(raptor, Raptor)
#endif
