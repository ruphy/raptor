/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>
   Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "raptorgraphicswidget.h"

// Local
#include "view/raptoritemsview.h"
#include "view/raptoritemdelegate.h"
#include "view/raptorscrollbutton.h"
#include "engine/kickoff/applicationmodel.h"
#include "engine/kickoff/searchmodel.h"
#include "view/raptorbreadcrumb.h"
#include "view/raptorgraphicsview.h"

#include "playground/breadcrumb.h"

// Qt
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneResizeEvent>
#include <QScrollBar>
#include <QPainter>

// KDE
#include <KDesktopFile>
#include <KRun>
#include <KService>
#include <KServiceTypeTrader>
#include <KConfig>
#include <KDebug>

// Plasma
#include <Plasma/Theme>
#include <Plasma/LineEdit>
#include <Plasma/Applet>
#include <plasma/runnermanager.h>

class RaptorGraphicsWidget::Private
{
public:
    Private(RaptorGraphicsWidget *q) : q(q),
                                       view(0),
                                       proxy(0),
                                       model(0),
                                       searchModel(0),
                                       rightScrollButton(0),
                                       rightScrollButtonProxy(0),
                                       leftScrollButton(0),
                                       leftScrollButtonProxy(0)
    {}
    ~Private()
    {
    }

    RaptorGraphicsWidget *q;
    RaptorGraphicsView *view;
    QGraphicsProxyWidget *proxy;
    Kickoff::ApplicationModel *model;
    Kickoff::SearchModel * searchModel;
    RaptorScrollButton *rightScrollButton;
    QGraphicsProxyWidget *rightScrollButtonProxy;
    RaptorScrollButton *leftScrollButton;
    QGraphicsProxyWidget *leftScrollButtonProxy;
    Plasma::LineEdit *searchLine;
    Breadcrumb * breadCrumb;
    Plasma::RunnerManager * manager;
    KConfigGroup appletConfig;
};

RaptorGraphicsWidget::RaptorGraphicsWidget(QGraphicsItem *parent, const KConfigGroup &appletconfig)
    : QGraphicsWidget(parent),
      d(new Private(this))
{
    setAcceptHoverEvents(true);

    d->model = new Kickoff::ApplicationModel();
    d->model->init();
    d->searchModel = new Kickoff::SearchModel();

    d->view = new RaptorGraphicsView(this);//Initialize the view as first element, some depend on it
    d->view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->view->setModel(d->model);

    d->leftScrollButton = new RaptorScrollButton(RaptorScrollButton::Left, this);
//     d->view = new RaptorItemsView();
//     RaptorItemDelegate *delegate = new RaptorItemDelegate();

    d->breadCrumb = new Breadcrumb(d->model, this);
    d->searchLine = new Plasma::LineEdit(this);
    d->searchLine->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    d->rightScrollButton = new RaptorScrollButton(RaptorScrollButton::Right, this);
    d->appletConfig = appletconfig;

    QGraphicsLinearLayout *verticalLayout = new QGraphicsLinearLayout(Qt::Vertical);

    QGraphicsLinearLayout *horizontalLayout = new QGraphicsLinearLayout();
    horizontalLayout->addItem(d->breadCrumb);
    //horizontalLayout->addStretch();
    horizontalLayout->addItem(d->searchLine);

    verticalLayout->addItem(horizontalLayout);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
    layout->setOrientation(Qt::Horizontal);

    connect(d->leftScrollButton, SIGNAL(clicked()), d->view, SLOT(scrollLeft()));
    layout->addItem(d->leftScrollButton);

    layout->addItem(d->view);

    connect(d->rightScrollButton, SIGNAL(clicked()), d->view, SLOT(scrollRight()));
    layout->addItem(d->rightScrollButton);

    verticalLayout->addItem(layout);

    setLayout(verticalLayout);
// 
//     delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
// 
//     // let's make the view nicer in the applet
//     d->view->setAttribute(Qt::WA_NoSystemBackground);
//     d->view->viewport()->setAutoFillBackground(true);
//     QPalette p = d->view->viewport()->palette();
//     p.setColor(QPalette::Base, Qt::transparent);
//     d->view->viewport()->setPalette(p);
// 
//     d->view->setModel(d->model);
//     d->view->setItemDelegate(delegate);
// 
//     d->view->hideScrollBars();
// 
//     d->proxy = new QGraphicsProxyWidget(this);
//    d->proxy->setWidget(d->view);

    KConfigGroup config(&d->appletConfig, "PlasmaRunnerManager");
    KConfigGroup conf(&config, "Plugins");

    conf.writeEntry("servicesEnabled", true);

    KService::List offers = KServiceTypeTrader::self()->query("Plasma/Runner");

    foreach (const KService::Ptr &service, offers) {
        KPluginInfo description(service);
        QString runnerName = description.pluginName();

        if (runnerName != "services")
        {
            conf.writeEntry(QString(runnerName + "Enabled"), false);
        }
    }

    conf.sync();
    config.sync();

    d->manager = new Plasma::RunnerManager(config, this);
    d->manager->reloadConfiguration();
// 
//     layout->addItem(d->proxy);
// 

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));
//     connect(d->view, SIGNAL(applicationClicked(const KUrl &)), this, SLOT(launchApplication(const KUrl &)));
    connect(d->searchLine, SIGNAL(textEdited(const QString&)), this, SLOT(refineModel()));
    connect(d->manager, SIGNAL(matchesChanged(const QList<Plasma::QueryMatch>&)), this,
            SLOT(matchesChanged(const QList<Plasma::QueryMatch>&)));

    connect(d->view, SIGNAL(enteredItem(const QModelIndex &)), d->breadCrumb, SLOT(setCurrentItem(const QModelIndex &)));
    connect(d->breadCrumb, SIGNAL(changedRootIndex(const QModelIndex&)), d->view, SLOT(setRootIndex(const QModelIndex &)));
//     connect(d->breadCrumb, SIGNAL(bottomLevelReached()), d->model, SLOT(slotReloadMenu()));
// 
//     d->view->focusCentralItem();

}

RaptorGraphicsWidget::~RaptorGraphicsWidget()
{
    delete d;
}

void RaptorGraphicsWidget::launchApplication(const KUrl &url)
{
    KDesktopFile desktopFile(url.pathOrUrl());
    KService service(&desktopFile);
    KRun::run(service, KUrl::List(), 0);
}

void RaptorGraphicsWidget::updateColors()
{
    //static_cast<RaptorItemDelegate*>(d->view->itemDelegate())->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
}

// QSizeF RaptorGraphicsWidget::sizeHint(Qt::SizeHint which, const QSizeF & constraint ) const
// {
//     QSizeF size;
//
//     kDebug()<<"minimum"<<d->view->minimumSize();
//     kDebug()<<"hint"<<d->view->sizeHint();
//     kDebug()<<"maximum"<<d->view->maximumSize();
//     kDebug()<<"current"<<this->size().toSize();
//
//     switch (which) {
//     case Qt::MinimumSize :
//         size = QSizeF(200,100);
//         break;
// //     default :
//     case Qt::PreferredSize :
//         size = QSizeF(d->view->sizeHint());
//         break;
//     case Qt::MaximumSize :
//         size = QSizeF(d->view->maximumSize());
//         break;
//     }
//
//     return size;
// }

void RaptorGraphicsWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    kDebug()<<"preferredSize"<<preferredSize();
    kDebug()<<"maximumSize"<<maximumSize();
    kDebug()<<"minimumSize"<<minimumSize();

    QGraphicsWidget::resizeEvent(event);
}

void RaptorGraphicsWidget::refineModel()
{
    if ( d->searchLine->text().isEmpty() ) {
        d->manager->reset();

        if (d->view->model() != d->model) {
            d->view->setModel(d->model);
        }
        return;
    }

    if (d->view->model() != d->searchModel) {
        d->view->setModel(d->searchModel);
    }

    d->manager->launchQuery(d->searchLine->text());
}

void RaptorGraphicsWidget::matchesChanged(const QList<Plasma::QueryMatch> &matches)
{
    d->searchModel->clearModelData();

    foreach (const Plasma::QueryMatch &ent, matches)  {
        KService::Ptr service = KService::serviceByStorageId(ent.data().toString());

        if(service) {
            d->searchModel->addAppNode(service);
        }
    }
}

// void RaptorGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
// {
// //     Q_UNUSED(option)
// //     Q_UNUSED(widget)
// //     painter->fillRect(rect(), Qt::red);
//     QGraphicsWidget::paint(painter, option, widget);
// }

#include "raptorgraphicswidget.moc"
