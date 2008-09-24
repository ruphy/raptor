/*
    Copyright 2007 Robert Knight <robertknight@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Own
#include "searchmodel.h"

// Qt

// KDE
#include <KDebug>
#include <KMimeType>
#include <KServiceTypeTrader>

using namespace Kickoff;

SearchModel::SearchModel()
    : ApplicationModel()
{

}
SearchModel::~SearchModel()
{
    delete d;
}
void SearchModel::resultsAvailable(const QStringList& results)
{
    SearchInterface *iface = qobject_cast<SearchInterface*>(sender());

    Q_ASSERT(iface);

    foreach(const QString& result,results) {
        //kDebug() << "Search hit from" << iface->name() << result;
        QStandardItem *resultItem = StandardItemFactory::createItemForUrl(result);
        d->addItemForIface(iface,resultItem);
    }
}

#include "searchmodel.moc"
