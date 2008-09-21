/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

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

// Qt
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneResizeEvent>
#include <QScrollBar>

// KDE
#include <KDesktopFile>
#include <KRun>
#include <KService>
#include <KDebug>

// Plasma
#include <Plasma/Theme>

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
    RaptorScrollButton *rightScrollButton;
    QGraphicsProxyWidget *rightScrollButtonProxy;
    RaptorScrollButton *leftScrollButton;
    QGraphicsProxyWidget *leftScrollButtonProxy;
};

RaptorGraphicsWidget::RaptorGraphicsWidget(QGraphicsItem *parent) : QGraphicsWidget(parent),
                                                                      d(new Private(this))
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(this);
    layout->setOrientation(Qt::Horizontal);

    d->leftScrollButton = new RaptorScrollButton(RaptorScrollButton::Left);
    d->leftScrollButtonProxy = new QGraphicsProxyWidget(this);
    d->leftScrollButtonProxy->setWidget(d->leftScrollButton);
    connect(d->leftScrollButton, SIGNAL(clicked()), SLOT(scrollLeft()));
    layout->addItem(d->leftScrollButtonProxy);

    d->view = new RaptorItemsView();
    RaptorItemDelegate *delegate = new RaptorItemDelegate();

    delegate->setTextColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));

    d->model = new Kickoff::ApplicationModel();

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

    layout->addItem(d->proxy);

    d->rightScrollButton = new RaptorScrollButton(RaptorScrollButton::Right);
    connect(d->rightScrollButton, SIGNAL(clicked()), SLOT(scrollRight()));
    d->rightScrollButtonProxy = new QGraphicsProxyWidget(this);
    d->rightScrollButtonProxy->setWidget(d->rightScrollButton);
    layout->addItem(d->rightScrollButtonProxy);

    setLayout(layout);

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(updateColors()));
    connect(d->view, SIGNAL(applicationClicked(const KUrl &)), this, SLOT(launchApplication(const KUrl &)));

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
