/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans <l.appelhans@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.
*/
#include "RaptorEngine.h"

#include <QStandardItemModel>
#include <QStandardItem>

RaptorEngine::RaptorEngine(QObject* parent, const QVariantList& args)
  : Plasma::DataEngine(parent, args)
{
    QStandardItemModel * model = new QStandardItemModel(this);
    for (int i = 0; i != 15; i++)
    {
        model->appendRow(new QStandardItem("blub" + QString::number(i)));
    }

    QVariant variant;
    variant.setValue(model);
    setData("model", variant);
}

RaptorEngine::~RaptorEngine()
{
}

#include "RaptorEngine.moc"
