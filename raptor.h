/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

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
#include <Plasma/Svg>

class RaptorGraphicsWidget;

class QSizeF;

// Define our plasma Applet
class Raptor : public Plasma::PopupApplet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        Raptor(QObject *parent, const QVariantList &args);
        ~Raptor();

        // The paintInterface procedure paints the applet to screen
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);

        void init();

        QGraphicsWidget *graphicsWidget();

    protected:
        void setupView();


    private:
        Plasma::Svg m_svg;
        KIcon m_icon;
        RaptorGraphicsWidget *m_gwidget;
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(raptor, Raptor)
#endif
