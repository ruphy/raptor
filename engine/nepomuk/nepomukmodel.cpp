#include "nepomukmodel.h"

#include "task.h"
#include "application.h"
#include "applicationlaunch.h"

#include <QList>
#include <QDateTime>

#include <KServiceTypeTrader>
#include <KService>
#include <KIcon>
#include <KNotification>
#include <KDebug>
#include <KServiceGroup>
#include <KSycocaEntry>

namespace Raptor {
class MenuItem
{
public:
    MenuItem(MenuItem * parent = 0) : m_parent(parent) {}
    virtual ~MenuItem() {}
    
    virtual NepomukModel::EntryType entryType() = 0;
    virtual QVariant data(int role) = 0;
    MenuItem* parent() { return m_parent; }
    
private:
    MenuItem * m_parent;
};

class Application : public MenuItem
{
public:
    Application(Nepomuk::Application &a, MenuItem * parent) : MenuItem(parent), app(a) {}
    virtual ~Application() {}
    
    virtual NepomukModel::EntryType entryType() { return NepomukModel::Application; }
    virtual QVariant data(int role) {
        if (role == Qt::DisplayRole) {
	    return app.label();
	} else if (role == Qt::DecorationRole) {
	    return KIcon(app.symbols().first());
	} else if (role == NepomukModel::UrlRole) {
	    return app.desktopFilePath();
	} else if (role == NepomukModel::NepomukUriRole) {
	    return app.resourceUri();
	} else if (role == NepomukModel::LastLaunchedRole) {
	    return app.lastLaunch().launchDate();
	} else if (role == NepomukModel::LaunchCountRole) {
	    return app.launchCount();
	}
	return QVariant();
    }
	  
private:
    Nepomuk::Application app;
};

class MenuTask : public MenuItem
{
public:
    MenuTask(Nepomuk::Task &t, MenuItem * parent) : MenuItem(parent), m_task(t) {
        foreach (Nepomuk::Application app, t.applications()) {
	    m_apps.append(new Application(app, this));
	}
    }
    virtual ~MenuTask() {
        qDeleteAll(m_apps);
    }
    
    virtual NepomukModel::EntryType entryType() { return NepomukModel::Task; }
    virtual QVariant data(int role) {
        if (role == Qt::DisplayRole) {
	    return m_task.label();
	} else if (role == Qt::DecorationRole) {
	    return KIcon(m_task.symbols().first());
	} else if (role == NepomukModel::NepomukUriRole) {
	    return m_task.resourceUri();
	}
	return QVariant();
    }
    QList<Application*> applications() { return m_apps; }
    
private:
    Nepomuk::Task m_task;
    QList<Application*> m_apps;
};

class MenuCategory : public MenuItem
{
public:
    MenuCategory(Nepomuk::Category &c) : MenuItem(), m_category(c) {
        foreach (Nepomuk::Task task, m_category.tasks()) {
	    m_tasks.append(new MenuTask(task, this));
	}
    }
    virtual ~MenuCategory() {
        qDeleteAll(m_tasks);
    }
    
    virtual NepomukModel::EntryType entryType() { return NepomukModel::Category; }
    virtual QVariant data(int role) {
        if (role == Qt::DisplayRole) {
	    return m_category.label();
	} else if (role == Qt::DecorationRole) {
	    return KIcon(m_category.symbols().first());
	} else if (role == NepomukModel::NepomukUriRole) {
	    return m_category.resourceUri();
	}
	return QVariant();
    }
    QList<MenuTask*> tasks() { return m_tasks; }
    
private:
    Nepomuk::Category m_category;
    QList<MenuTask*> m_tasks;
};

class NepomukModel::Private
{
public:
    Private() {}
    ~Private() { qDeleteAll(categories); }

    QList<MenuCategory*> categories;
};

NepomukModel::NepomukModel(QObject *parent)
  : QAbstractItemModel(parent),
    d(new Private())
{
    KService::List services = KServiceTypeTrader::self()->query("Application");
    QList<Nepomuk::Application> apps = Nepomuk::Application::allApplications();
    if (apps.isEmpty()) {
        KNotification::event(i18n("Building database..."));
    }
    
    foreach (Nepomuk::Application app, apps) {
        bool found = false;
	foreach (KService::Ptr service, services) {
	    if (service->desktopEntryName() == app.serviceName()) {
	        found = true;
		services.removeAll(service);
		break;
	    }
	}
	app.setIsInstalled(found);
    }
    //Now we add not found services to the database
    //foreach (KService::Ptr service, services) {#
    for (int i = 0; i != services.count(); i++) {
        KService::Ptr service = services[i];
        if (service->noDisplay()) {
	    continue;
	}
        Nepomuk::Application app(service->desktopEntryName());
	app.setServiceName(service->desktopEntryName());
        app.setLabel(service->name());
	app.setIsInstalled(true);
	app.setLaunchCount(0);
// 	KDesktopFile desktopFile(service->desktopEntryName());
	app.setSymbols(QStringList() << service->icon());
	app.setDesktopFilePath(service->entryPath());
	
	Nepomuk::Task task(service->icon());
	task.setLabel(service->icon());
	task.setSymbols(QStringList() << service->icon());
	task.setApplications(QList<Nepomuk::Application>() << app);
	Nepomuk::Category category(service->categories().isEmpty() ? "bla" : service->categories().first());
	category.setLabel(service->categories().isEmpty() ? "bla" : service->categories().first());
	category.setSymbols(QStringList() << "help");
	category.setTasks(QList<Nepomuk::Task>() << task);
	//app.setCategories(service->categories());
// 	app.setDescription
    }
    d->categories.clear();
    foreach (Nepomuk::Category cat, Nepomuk::Category::allCategorys()) {
        d->categories.append(new MenuCategory(cat));
    }
    
    //m_apps = Nepomuk::Application::allApplications();
    
    /*KServiceGroup::Ptr root = KServiceGroup::group(QString());
    if (!root || !root->isValid()) return;

    KServiceGroup::List list = root->entries();
    
    foreach (KSharedPtr<KSycocaEntry> entry, list) {
        d->categories.append(entry->name());
    }
    kDebug() << d->categories;*/
}

NepomukModel::~NepomukModel()
{
    delete d;
}

int NepomukModel::columnCount(const QModelIndex & parent) const
{
    return 0;
}

QVariant NepomukModel::data(const QModelIndex & index, int role) const
{
    MenuItem * item = static_cast<MenuItem*>(index.internalPointer());
    return item->data(role);
}

QModelIndex NepomukModel::index(int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    if (column > 0 || row < 0) {
        return QModelIndex();
    }
    
    if (!parent.isValid() && d->categories.count() > row) {
        return createIndex(row, column, d->categories[row]);
    } else {
        MenuItem *item = static_cast<MenuItem*>(parent.internalPointer());
	if (item->entryType() == Category && dynamic_cast<MenuCategory*>(item)->tasks().count() > row) {
	    return createIndex(row, column, dynamic_cast<MenuCategory*>(item)->tasks()[row]);
	} else if (item->entryType() == Task && dynamic_cast<MenuTask*>(item)->applications().count() > row) {
	    return createIndex(row, column, dynamic_cast<MenuTask*>(item)->applications()[row]);
	}
    }
    return QModelIndex();
}

QModelIndex NepomukModel::parent(const QModelIndex & index) const
{
    MenuItem *item = static_cast<MenuItem*>(index.internalPointer());
    if (item->entryType() == Application) {
        return createIndex(dynamic_cast<MenuCategory*>(item->parent()->parent())->tasks().indexOf(dynamic_cast<MenuTask*>(item->parent())), 0, item->parent());
    } else if (item->entryType() == Task) {
        return createIndex(d->categories.indexOf(dynamic_cast<MenuCategory*>(item->parent())), 0, item->parent());
    }   
    return QModelIndex();
}

int NepomukModel::rowCount(const QModelIndex & parent) const
{
    MenuItem * item = static_cast<MenuItem*>(parent.internalPointer());
    if (!parent.isValid()) {
        return d->categories.count();
    } else if (item->entryType() == Category) {
        return dynamic_cast<MenuCategory*>(item)->tasks().count();
    } else if (item->entryType() == Task) {
        return dynamic_cast<MenuTask*>(item)->applications().count();
    }
    return 1;
}
}
