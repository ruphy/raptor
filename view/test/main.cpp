/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/ 

#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>
#include <KDialog>

#include "view/raptoritemsview.h"
#include "view/raptoritemdelegate.h"
#include "model/applicationmodel.h"


/**
 * A simple test application for RaptorItemsView
 */

int main(int argc, char *argv[])
{
    KAboutData aboutData("itemview-test", 0, ki18n("test for item view"),
                         "0.1", ki18n("test app"),
                         KAboutData::License_GPL,
                         ki18n("(c) 2008 Alessandro Diaferia"),
                         KLocalizedString(), "", "alediaferia@gmail.com");
    aboutData.addAuthor(ki18n("Alessandro Diaferia"), KLocalizedString(), "alediaferia@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::parsedArgs();
    KApplication app;

    KDialog *window = new KDialog();
    RaptorItemsView *itemsView = new RaptorItemsView(window);
    Kickoff::ApplicationModel *model = new Kickoff::ApplicationModel();
    itemsView->setModel(model);
    RaptorItemDelegate *delegate = new RaptorItemDelegate();
    itemsView->setItemDelegate(delegate);

    window->setMainWidget(itemsView);

    window->show();

   return app.exec();
    
}
