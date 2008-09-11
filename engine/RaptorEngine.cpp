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
    qRegisterMetaType<Kickoff::ApplicationModel*>();
    QVariant variant;
    variant.setValue(new Kickoff::ApplicationModel(this));
    setData("model", variant);
}

RaptorEngine::~RaptorEngine()
{
}

#include "RaptorEngine.moc"
