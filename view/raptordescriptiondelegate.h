/* This file is part of the KDE project

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef RAPTORDESCRIPTIONDELEGATE_H
#define RAPTORDESCRIPTIONDELEGATE_H

#include <QStyledItemDelegate>

class RaptorDescriptionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
        RaptorDescriptionDelegate(QObject *parent = 0);
        ~RaptorDescriptionDelegate();

        void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
        void setTextColor(const QColor &);


    private:
        class Private;
        Private *d;
};

#endif
