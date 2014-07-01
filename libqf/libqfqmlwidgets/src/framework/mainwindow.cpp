#include "mainwindow.h"
#include "application.h"
#include "pluginloader.h"
#include "../menubar.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/utils.h>
//#include <qf/core/settings.h>
//#include <qf/core/utils/crypt.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QSettings>

using namespace qf::qmlwidgets::framework;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	QMainWindow(parent, flags), IPersistentSettings(this)
{
	m_pluginLoader = nullptr;

	QQmlEngine *qe = Application::instance()->qmlEngine();
	qe->rootContext()->setContextProperty("FrameWork", this);

	MenuBar *main_menu = new MenuBar(this);
	setMenuBar(main_menu);
}

MainWindow::~MainWindow()
{
	savePersistentSettings();
}

void MainWindow::loadPlugins()
{
	QF_SAFE_DELETE(m_pluginLoader);
	m_pluginLoader = new PluginLoader(this);
	connect(m_pluginLoader, &PluginLoader::loadingFinished, this, &MainWindow::pluginsLoaded, Qt::QueuedConnection);
	m_pluginLoader->loadPlugins();
}

void MainWindow::loadPersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		QRect geometry = settings.value("geometry").toRect();
		if(geometry.isValid()) {
			this->setGeometry(geometry);
		}
	}
}

class TestObject : public QObject
{
	Q_OBJECT
public:
	TestObject(QObject *parent = 0) : QObject(parent)
	{
		static int cnt = 0;
		setObjectName(QString("OBJ%1").arg(++cnt));
		//QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
		qfInfo() << "CREATE" << this << "ownersip:" << QQmlEngine::objectOwnership(this);
	}
	~TestObject()
	{
		qfInfo() << "DESTROY" << this << "ownersip:" << QQmlEngine::objectOwnership(this);
	}
	Q_INVOKABLE QObject* parent() {return QObject::parent();}
};

QObject *MainWindow::obj_testing()
{
	QObject *ret = new TestObject();
	return ret;
}

void MainWindow::savePersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QRect geometry = this->geometry();
		QSettings settings;
		settings.beginGroup(path);
		settings.setValue("geometry", geometry);
	}
}

void MainWindow::setPersistentSettingDomains(const QString &organization_domain, const QString &organization_name, const QString &application_name)
{
	QCoreApplication::setOrganizationDomain(organization_domain);
	QCoreApplication::setOrganizationName(organization_name);
	if(!application_name.isEmpty()) {
		QCoreApplication::setApplicationName(application_name);
	}
	//QSettings::setDefaultFormat(QSettings::IniFormat);
}

/*
void MainWindow::setupSettingsPersistence()
{
	QString path = persistentSettingsPath();
	if(path.isEmpty()) {
		disconnect(this, SIGNAL(destroyed()), this, SLOT(savePersistentSettings()));
	}
	else {
		QObject::connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(savePersistentSettings()), Qt::UniqueConnection);
		QMetaObject::invokeMethod(this, "loadPersistentSettings", Qt::QueuedConnection);
	}
}
*/

qf::qmlwidgets::MenuBar *MainWindow::menuBar()
{
	return qobject_cast<MenuBar*>(Super::menuBar());
}

QObject *MainWindow::plugin(const QString &feature_id)
{
	QObject *ret = nullptr;
	if(m_pluginLoader) {
		ret = m_pluginLoader->loadedPlugins().value(feature_id);
	}
	if(!ret) {
		qfWarning() << "Plugin for feature id:" << feature_id << "is not installed!";
	}
	return ret;
}

#include "mainwindow.moc"
