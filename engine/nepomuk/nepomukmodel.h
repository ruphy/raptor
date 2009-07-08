#ifndef RAPTORNEPOMUKMODEL_H
#define RAPTORNEPOMUKMODEL_H

#include <QAbstractItemModel>

#include "category.h"

class KService;
namespace Raptor {
/**
 * This method returns the associated Category and Task
 * as from the TOM spec you can find along with raptor.
 * The first QString is the Category and the second one is
 * the Task.
 */
static QHash<QString, QString> tomFromService(KService * service);
class NepomukModel : public QAbstractItemModel
{
Q_OBJECT
public:
    enum DataRole
    {
        /** A sub title to be displayed below the text from the item's Qt::DisplayRole data */
        SubTitleRole = Qt::UserRole+1,
        FirstDataRole = SubTitleRole,
        /** The URL to be opened when executing the item. */
        UrlRole = Qt::UserRole + 2,
        /** The Solid device identifier for items which represent devices. */
        DeviceUdiRole = Qt::UserRole + 3,
        /** The amount of space (in Kilobytes) used for items which represent storage. */
        DiskUsedSpaceRole = Qt::UserRole + 4,
        /** The amount of free space (in Kilobytes) for items which represent storage. */
        DiskFreeSpaceRole = Qt::UserRole + 5,
        SubTitleMandatoryRole = Qt::UserRole + 6,
        LastDataRole = SubTitleMandatoryRole,
	NepomukUriRole = Qt::UserRole + 7,
	LastLaunchedRole = Qt::UserRole + 8,
	LaunchCountRole = Qt::UserRole + 9
    };
    enum EntryType
    {
        Category,
	Task,
	Application
    };
    NepomukModel(QObject *parent = 0);
    ~NepomukModel();
    
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex & index) const;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    
private:
    class Private;
    Private *d;
};
}


#endif
