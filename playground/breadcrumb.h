/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef BREADCRUMB_H
#define BREADCRUMB_H

#include <QGraphicsWidget>
#include <QList>

class BreadcrumbItem;
class QModelIndex;
class QAbstractItemModel;

class Breadcrumb : QGraphicsWidget
{
    Q_OBJECT
public:
    Breadcrumb(QAbstractItemModel *model, QGraphicsWidget *parent = 0);
    ~Breadcrumb();

public slots:
    void setCurrentItem(const QModelIndex &index);

signals:
    /**
     * This signal is emitted when the user clicks an
     * item of the breadcrumb.
     */
    void changedRootIndex(const QModelIndex &);

private:
    QAbstractItemModel *m_model;
    QList<BreadcrumbItem*> m_items;
};

#endif
