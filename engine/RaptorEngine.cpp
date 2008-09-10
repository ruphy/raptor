/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>
   Copyright (C) 2008 Dario Freddi <drf@kdemod.ath.cx>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "RaptorEngine.h"

RaptorEngine::RaptorEngine(QObject* parent, const QVariantList& args)
  : Plasma::DataEngine(parent, args)
{
    setMinimumPollingInterval(2000);

    qRegisterMetaType<Kickoff::ApplicationModel*>();
    
    m_model = new Kickoff::ApplicationModel(this);
}

RaptorEngine::~RaptorEngine()
{
}

QStringList RaptorEngine::sources() const
{
    QStringList sources;
    sources << "Raptor";

    return sources;
}

void RaptorEngine::setRefreshTime( uint time )
{
    setPollingInterval( time );
}

uint RaptorEngine::refreshTime() const
{
    return 1000;
}

bool RaptorEngine::sourceRequestEvent( const QString &name )
{
    return updateSourceEvent( name );
}

bool RaptorEngine::updateSourceEvent( const QString &name )
{
    if ( !name.compare( "Raptor", Qt::CaseInsensitive ) )
        getRaptorData( name );

    return true;
}

void RaptorEngine::getRaptorData( const QString &name )
{
    removeAllData( name );

    QVariant v;

    v.setValue(m_model);

    setData( name, "model", v );
}

void RaptorEngine::updateRaptorData()
{
    getRaptorData( "Raptor" );
}

#include "RaptorEngine.moc"
