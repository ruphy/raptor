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

// Qt
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneResizeEvent>
#include <QScrollBar>

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
                                       proxy(0)
    {}
    ~Private()
    {
    }

    RaptorGraphicsWidget *q;
    RaptorItemsView *view;
    QGraphicsProxyWidget *proxy;
    Kickoff::ApplicationModel *model;
    Kickoff::SearchModel * searchModel;
    RaptorScrollButton *rightScrollButton;
    QGraphicsProxyWidget *rightScrollButtonProxy;
    RaptorScrollButton *leftScrollButton;
    QGraphicsProxyWidget *leftScrollButtonProxy;
    Plasma::LineEdit *searchLine;
    RaptorBreadCrumb * breadCrumb;
    Plasma::RunnerManager * manager;
    KConfigGroup appletConfig;
};

RaptorGraphicsWidget::RaptorGraphicsWidget(QGraphicsItem *parent, const KConfigGroup &appletconfig)
    : QGraphicsWidget(parent),
      d(new Private(this))
{
    d->leftScrollButton = new RaptorScrollButton(RaptorScrollButton::Left);
    d->view = new RaptorItemsView();
    RaptorItemDelegate *delegate = new RaptorItemDelegate();
    d->model = new Kickoff::ApplicationModel();
    d->model->init();
    d->searchModel = new Kickoff::SearchModel();
    d->breadCrumb = new RaptorBreadCrumb(d->view, d->model, this);
    d->searchLine = new Plasma::LineEdit(this);
    d->rightScrollButton = new RaptorScrollButton(RaptorScrollButton::Right);
    d->appletConfig = appletconfig;

    QGraphicsLinearLayout *verticalLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    QGraphicsLinearLayout *horizontalLayout = new QGraphicsLinearLayout(verticalLayout);
    verticalLayout->addItem(horizontalLayout);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(verticalLayout);
    layout->setOrientation(Qt::Horizontal);
    verticalLayout->addItem(layout);

    horizontalLayout->addItem(d->breadCrumb);
    horizontalLayout->addStretch();
    horizontalLayout->addItem(d->searchLine);

    d->leftScrollButtonProxy = new QGraphicsProxyWidget(this);
    d->leftScrollButtonProxy->setWidget(d->leftScrollButton);
    connect(d->leftScrollButton, SIGNAL(clicked()), SLOT(scrollLeft()));
    layout->addItem(d->leftScrollButtonProxy);

    delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));

    // let's make the view nicer in the applet
    d->view->setAttribute(Qt::WA_NoSystemBackground);
    d->view->viewport()->setAutoFillBackground(true);
    QPalette p = d->view->viewport()->palette();
    p.setColor(QPalette::Base, Qt::transparent);
    d->view->viewport()->setPalette(p);

    d->view->setModel(d->model);
    d->view->setItemDelegate(delegate);

    d->view->hideScrollBars();

    d->proxy = new QGraphicsProxyWidget(this);
    d->proxy->setWidget(d->view);

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

    layout->addItem(d->proxy);

    connect(d->rightScrollButton, SIGNAL(clicked()), SLOT(scrollRight()));
    d->rightScrollButtonProxy = new QGraphicsProxyWidget(this);
    d->rightScrollButtonProxy->setWidget(d->rightScrollButton);
    layout->addItem(d->rightScrollButtonProxy);

    setLayout(verticalLayout);

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));
    connect(d->view, SIGNAL(applicationClicked(const KUrl &)), this, SLOT(launchApplication(const KUrl &)));
    connect(d->searchLine, SIGNAL(textEdited(const QString&)), this, SLOT(refineModel()));
    connect(d->manager, SIGNAL(matchesChanged(const QList<Plasma::QueryMatch>&)), this,
            SLOT(matchesChanged(const QList<Plasma::QueryMatch>&)));

    d->view->focusCentralItem();
}

RaptorGraphicsWidget::~RaptorGraphicsWidget()
{
    delete d;
}

void RaptorGraphicsWidget::launchApplication(const KUrl &url)
{
    KDesktopFile desktopFile(url.pathOrUrl());
    KService service(&desktopFile);
    KRun::run(service, KUrl::List(), d->view);
}

void RaptorGraphicsWidget::scrollLeft()
{
    QModelIndex selected = d->view->currentIndex();

    int rowCount = d->model->rowCount();
    int nextRow = selected.row()-1;
    if(nextRow<0)
        nextRow = rowCount-1;

    QModelIndex leftOne = d->model->index(nextRow, 0);
    d->view->setCurrentIndex(leftOne);
    d->view->update();
}

void RaptorGraphicsWidget::scrollRight()
{
    QModelIndex selected = d->view->currentIndex();

    int rowCount = d->model->rowCount();
    int nextRow = selected.row()+1;
    if(nextRow>rowCount-1)
        nextRow = 0;

    QModelIndex rightOne = d->model->index(nextRow, 0);
    d->view->setCurrentIndex(rightOne);
    d->view->update();
}

void RaptorGraphicsWidget::updateColors()
{
    static_cast<RaptorItemDelegate*>(d->view->itemDelegate())->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
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
    kDebug()<<"oldSize"<<event->oldSize();
    kDebug()<<"newSize"<<event->newSize();
    kDebug()<<"preferredSize"<<preferredSize();
    kDebug()<<"maximumHeight"<<maximumHeight();

    QGraphicsWidget::resizeEvent(event);
}

void RaptorGraphicsWidget::refineModel()
{
    if ( d->searchLine->text().isEmpty() )
    {
        d->manager->reset();

        if (d->view->model() != d->model)
        {
            d->view->setModel(d->model);
        }
        return;
    }

    if (d->view->model() != d->searchModel)
    {
        d->view->setModel(d->searchModel);
    }

    d->manager->launchQuery(d->searchLine->text());
}

void RaptorGraphicsWidget::matchesChanged(const QList<Plasma::QueryMatch> &matches)
{
    d->searchModel->clearModelData();

    foreach (const Plasma::QueryMatch &ent, matches)
    {
        KService::Ptr service = KService::serviceByStorageId(ent.data().toString());

        if(service)
        {
            d->searchModel->addAppNode(service);
        }
    }
}

#include "raptorgraphicswidget.moc"
