/*
  Copyright (c) 2008 Riccardo Iaconelli <riccardo@kde.org>

            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QtGui>//TODO: Don't include whole module

#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>

//#include "widget.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("widget-test", 0, ki18n("test for widget"),
                         "0.1", ki18n("Description"), 
                         KAboutData::License_GPL,
                         ki18n("(c) 2008  Riccardo Iaconelli"), 
                         KLocalizedString(), "", "riccardo@kde.org");
    aboutData.addAuthor(ki18n("Riccardo Iaconelli"), KLocalizedString(), "riccardo@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::parsedArgs();
    KApplication app;

    QGraphicsScene s;
    QGraphicsView v(&s);
    
    v.show();
    
    return app.exec();
}

