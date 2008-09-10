/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef RAPTORENGINE_H
#define RAPTORENGINE_H

#include <plasma/dataengine.h>
#include <QStandardItemModel>

class RaptorEngine : public Plasma::DataEngine
{
    Q_OBJECT
    public:
        RaptorEngine(QObject* parent, const QVariantList& args);
        ~RaptorEngine();
};

Q_DECLARE_METATYPE(QStandardItemModel*)
K_EXPORT_PLASMA_DATAENGINE(raptor, RaptorEngine)

#endif
