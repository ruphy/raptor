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
#include "engine/kickoff/favoritesmodel.h"
#include "view/raptorgraphicsview.h"

#include "view/breadcrumb.h"

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
#include <KIcon>

// Plasma
#include <Plasma/Theme>
#include <Plasma/LineEdit>
#include <Plasma/IconWidget>
#include <Plasma/Applet>
#include <Plasma/FrameSvg>
#include <plasma/runnermanager.h>

const int CONTENTS_RECT_HORIZONTAL_MARGIN = 32;
const int CONTENTS_RECT_VERTICAL_MARGIN = 10;
const int FRAME_RECT_HORIZONTAL_MARGIN = 22;
const int FRAME_RECT_VERTICAL_MARGIN = 8;

class RaptorGraphicsWidget::Private
{
public:
    Private(RaptorGraphicsWidget *q) : q(q),
                                       view(0),
                                       model(0),
                                       searchModel(0),
                                       rightScrollButton(0),
                                       leftScrollButton(0)
    {
        frame = new Plasma::FrameSvg(q);
        frame->setImagePath("dialogs/background");
        frame->setEnabledBorders(Plasma::FrameSvg::AllBorders);
    }
    ~Private()
    {
        delete searchModel;
    }

    RaptorGraphicsWidget *q;
    RaptorGraphicsView *view;

    Kickoff::ApplicationModel *model;
    Kickoff::SearchModel * searchModel;
    Kickoff::FavoritesModel * favoritesModel;

    RaptorScrollButton *rightScrollButton;
    RaptorScrollButton *leftScrollButton;
    Breadcrumb * breadCrumb;

    Plasma::LineEdit *searchLine;
    Plasma::IconWidget * favoritesIcon;
    Plasma::RunnerManager * manager;
    Plasma::FrameSvg *frame;

    KConfigGroup appletConfig;
};

RaptorGraphicsWidget::RaptorGraphicsWidget(QGraphicsItem *parent, const KConfigGroup &appletconfig)
    : QGraphicsWidget(parent),
      d(new Private(this))
{
    setAcceptHoverEvents(true);

    d->model = new Kickoff::ApplicationModel(this);
    d->model->init();
    d->searchModel = new Kickoff::SearchModel();
    d->favoritesModel = new Kickoff::FavoritesModel(this);

    d->view = new RaptorGraphicsView(this);//Initialize the view as first element, some depend on it
    d->view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->view->setModel(d->model);

    d->leftScrollButton = new RaptorScrollButton(RaptorScrollButton::Left, this);
//     d->view = new RaptorItemsView();
//     RaptorItemDelegate *delegate = new RaptorItemDelegate();

    d->breadCrumb = new Breadcrumb(d->view, this);
    d->searchLine = new Plasma::LineEdit(this);
    d->favoritesIcon = new Plasma::IconWidget(this);
    d->favoritesIcon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    d->favoritesIcon->setIcon(KIcon("rating"));
    //d->searchLine->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    d->rightScrollButton = new RaptorScrollButton(RaptorScrollButton::Right, this);


    d->appletConfig = appletconfig;

    QGraphicsLinearLayout *verticalLayout = new QGraphicsLinearLayout(Qt::Vertical);

    QGraphicsLinearLayout *horizontalLayout = new QGraphicsLinearLayout();
    horizontalLayout->addItem(d->breadCrumb);
    horizontalLayout->addStretch();
    horizontalLayout->addItem(d->favoritesIcon);
    horizontalLayout->addItem(d->searchLine);

    verticalLayout->addItem(horizontalLayout);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
    layout->setOrientation(Qt::Horizontal);

    connect(d->leftScrollButton, SIGNAL(clicked()), d->view, SLOT(scrollLeft()));
//     layout->addItem(d->leftScrollButton);

    layout->addItem(d->view);

    connect(d->rightScrollButton, SIGNAL(clicked()), d->view, SLOT(scrollRight()));
//     layout->addItem(d->rightScrollButton);

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
    connect(d->view, SIGNAL(applicationClicked(const KUrl &)), this, SLOT(launchApplication(const KUrl &)));
    connect(d->searchLine, SIGNAL(textEdited(const QString&)), this, SLOT(refineModel()));
    connect(d->manager, SIGNAL(matchesChanged(const QList<Plasma::QueryMatch>&)), this,
            SLOT(matchesChanged(const QList<Plasma::QueryMatch>&)));

    connect(d->view, SIGNAL(enteredItem(const QModelIndex &)), d->breadCrumb, SLOT(setCurrentItem(const QModelIndex &)));
    connect(d->view, SIGNAL(favoriteClicked(const QString &)), this, SLOT(addOrRemoveFavorite(const QString &)));
    connect(d->breadCrumb, SIGNAL(changedRootIndex(const QModelIndex&)), d->view, SLOT(setRootIndex(const QModelIndex &)));
    connect(d->breadCrumb, SIGNAL(rootMenuRequested()), SLOT(refineModel()));
    connect(d->favoritesIcon, SIGNAL(clicked()), SLOT(setFavoritesModel()));
//     connect(d->breadCrumb, SIGNAL(bottomLevelReached()), d->model, SLOT(slotReloadMenu()));
// 
//     d->view->focusCentralItem();

    setContentsMargins(CONTENTS_RECT_HORIZONTAL_MARGIN, CONTENTS_RECT_VERTICAL_MARGIN, CONTENTS_RECT_HORIZONTAL_MARGIN, CONTENTS_RECT_VERTICAL_MARGIN);

    d->rightScrollButton->resize(32, 32);
    d->leftScrollButton->resize(32, 32);
}

RaptorGraphicsWidget::~RaptorGraphicsWidget()
{
    delete d;
}

void RaptorGraphicsWidget::setFavoritesModel()
{
    if (d->view->model() == d->favoritesModel) {
        //d->view->setRootIndex(QModelIndex());
        refineModel();
    }
    else {
        d->view->setModel(d->favoritesModel);
        d->view->setRootIndex(d->favoritesModel->index(0, 0, QModelIndex()));
    }
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

void RaptorGraphicsWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    d->rightScrollButton->setPos(contentsRect().right() - d->rightScrollButton->size().width() / 2, rect().height() / 2 - d->rightScrollButton->rect().height() / 2);
    d->leftScrollButton->setPos(contentsRect().x() - d->leftScrollButton->size().width() / 2, rect().height() / 2 - d->leftScrollButton->rect().height() / 2);
}

void RaptorGraphicsWidget::refineModel()
{
    if ( d->searchLine->text().isEmpty() ) {
        d->manager->reset();

        if (d->view->model() != d->model) {
            d->view->setModel(d->model);
        } else {
            d->view->setRootIndex(QModelIndex());
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

void RaptorGraphicsWidget::addOrRemoveFavorite(const QString &url)
{
    if (Kickoff::FavoritesModel::isFavorite(url)) {
        Kickoff::FavoritesModel::remove(url);
    } else {
       Kickoff::FavoritesModel::add(url);
    }
    if (d->view->model() == d->favoritesModel) {
        d->view->reset();
    }
}

void RaptorGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

//     painter->fillRect(option->rect, Qt::red);

    QRectF frameRect(option->rect);
    frameRect.setX(FRAME_RECT_HORIZONTAL_MARGIN);
    //frameRect.setY(contentsRect().y());
    frameRect.setWidth(option->rect.width() - (2*FRAME_RECT_HORIZONTAL_MARGIN) );
    //frameRect.setHeight(option->rect.height() - (2*FRAME_RECT_VERTICAL_MARGIN));

//     painter->fillRect(contentsRect(), Qt::green);
//     painter->drawRect(frameRect);

    d->frame->resizeFrame(frameRect.size());
    d->frame->paintFrame(painter, frameRect.topLeft());
}
