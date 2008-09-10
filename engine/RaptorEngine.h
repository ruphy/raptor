/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf@kdemod.ath.cx>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORENGINE_H
#define RAPTORENGINE_H

#include <plasma/dataengine.h>
#include "kickoff/applicationmodel.h"

class RaptorEngine : public Plasma::DataEngine
{
    Q_OBJECT
    Q_PROPERTY( uint refreshTime READ refreshTime WRITE setRefreshTime )

    public:
        RaptorEngine(QObject* parent, const QVariantList& args);
        ~RaptorEngine();

        QStringList sources() const;

        void setRefreshTime( uint time );
        uint refreshTime() const;

    protected:
        bool sourceRequestEvent( const QString &name );
        bool updateSourceEvent( const QString& source );

    private slots:
        void getRaptorData( const QString &name );
        void updateRaptorData();

    private:
        Kickoff::ApplicationModel * m_model;
};

Q_DECLARE_METATYPE(Kickoff::ApplicationModel*)
K_EXPORT_PLASMA_DATAENGINE(raptor, RaptorEngine)

#endif
