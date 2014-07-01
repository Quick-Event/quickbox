#include "mainwindow.h"
#include "application.h"
#include "../menubar.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
//#include <qf/core/settings.h>
#include <qf/core/utils/crypt.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QDirIterator>
#include <QQmlComponent>
#include <QSettings>

//qf::core::utils::Crypt()

using namespace qf::qmlwidgets::framework;

static const char * prop_feature_id = "featureId";
static const char * prop_plugin_loader = "pluginLoader";
static const char * prop_plugin_path = "__pluginPath";
static const char * prop_depends_on_feature_ids = "dependsOnFeatureIds";
static const QLatin1String CoreFeatureId("Core");

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	QMainWindow(parent, flags), IPersistentSettings(this)
{
	MenuBar *main_menu = new MenuBar(this);
	setMenuBar(main_menu);
}

MainWindow::~MainWindow()
{
	savePersistentSettings();
}

void MainWindow::loadPlugins()
{
	qfLogFuncFrame();
	QQmlEngine *qe = application()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return);

	qe->rootContext()->setContextProperty("FrameWork", this);

	installPlugins(findPlugins());
}

MainWindow::PluginMap MainWindow::findPlugins()
{
	PluginMap ret;
	QQmlEngine *qe = application()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return ret);

	for(auto path : application()->qmlPluginImportPaths()) {
		qfInfo() << "Importing plugins on:" << path;
		QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks);
		while(it.hasNext()) {
			it.next();
			QFileInfo fi = it.fileInfo();
			if(QFile::exists(fi.absoluteFilePath() + "/qmldir")) {
				qfDebug() << "Trying to load plugin on path:" << fi.absoluteFilePath();
				QQmlComponent c(qe, QUrl::fromLocalFile(fi.absoluteFilePath() + "/Manifest.qml"));
				if(!c.isReady()) {
					qfError() << c.errorString();
				}
				else {
					QObject *root = c.create();
					if(!root) {
						qfError() << "Error creating plugin manifest:" << c.url().toString();
						qfError() << c.errorString();
					}
					else {
						QString feature_id = root->property(prop_feature_id).toString();
						if(feature_id.isEmpty()) {
							feature_id = fi.baseName();
							//qfInfo() << "FeatureId is empty, setting from plugin name to:" << feature_id;
							root->setProperty(prop_feature_id, feature_id);
						}
						QString plugin_loader = root->property(prop_plugin_loader).toString();
						if(plugin_loader.isEmpty()) {
							plugin_loader = "main.qml";
							//qfInfo() << "PluginLoader is empty, setting from plugin name to:" << plugin_loader;
							root->setProperty(prop_plugin_loader, plugin_loader);
						}
						QStringList depends_on_feature_ids = root->property(prop_depends_on_feature_ids).toStringList();
						if(feature_id != CoreFeatureId) {
							// each not Core feature implicitly depends on Core
							depends_on_feature_ids << CoreFeatureId;
							root->setProperty(prop_depends_on_feature_ids, depends_on_feature_ids);
						}

						QString plugin_path = fi.absoluteFilePath();
						root->setProperty(prop_plugin_path, plugin_path);
						qfInfo() << "Importing featureId:" << feature_id << "from plugin:" << fi.baseName() << "loader:" << plugin_loader;
						if(ret.contains(feature_id)) {
							qfError() << "Feature id:" << feature_id << "already loaded";
							delete root;
						}
						else {
							bool is_disabled = root->property("disabled").toBool();
							if(is_disabled) {
								qfInfo() << "Plugin featureId:" << feature_id << "DISABLED in manifest";
								delete root;
							}
							else {
								root->setParent(this);
								ret[feature_id] = root;
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

void MainWindow::installPlugins(const MainWindow::PluginMap &plugins_to_install)
{
	QQmlEngine *qe = application()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return);

	PluginMap to_install = plugins_to_install;
	while(!to_install.isEmpty()) {
		int cnt = to_install.count();
		QMutableMapIterator<QString, QObject*> it(to_install);
		while (it.hasNext()) {
		    it.next();
			QObject *manifest = it.value();
			QString feature_id = it.key();
		    QStringList depends_on = manifest->property(prop_depends_on_feature_ids).toStringList();
			bool dependency_satisfied = true;
			for(auto required_feature_id : depends_on) {
				if(!m_installedPlugins.contains(required_feature_id)) {
					dependency_satisfied = false;
					break;
				}
			}
			if(dependency_satisfied) {
				qfInfo() << "Installing feature:" << feature_id;
				QUrl plugin_loader_url = QUrl::fromLocalFile(manifest->property(prop_plugin_path).toString()
															 + "/"
															 + manifest->property(prop_plugin_loader).toString());
				QQmlComponent c(qe, plugin_loader_url);
				if(!c.isReady()) {
					qfError() << c.errorString();
				}
				else {
					Application *app = application();
					app->clearQmlErrorList();

					QObject *plugin = c.create();
					if(!plugin) {
						qfError() << "Error creating plugin manifest:" << c.url().toString();
						qfError() << c.errorString();
						manifest->deleteLater();
					}
					else {
						QVariant v = QVariant::fromValue(manifest);
						plugin->setProperty("manifest", v);
						plugin->setParent(this);
						m_installedPlugins[feature_id] = plugin;
					}

					if(app->qmlErrorList().count()) {
						qfError() << "Feature:" << feature_id << "install ERROR.";
					}
					else {
						qfInfo() << "Feature:" << feature_id << "install SUCCESS.";
					}
				}
				it.remove();
			}
		}
		if(cnt == to_install.count()) {
			break;
		}
	}
	if(!to_install.isEmpty()) {
		qfError() << "Features not installed due to unsatisfied dependeces:";
		QMapIterator<QString, QObject*> it(to_install);
		while (it.hasNext()) {
		    it.next();
			QObject *plugin = it.value();
			QString feature_id = it.key();
		    QStringList depends_on = plugin->property("dependsOnFeatureIds").toStringList();
			qfError() << "\t!!!" << feature_id << "depends on:" << CoreFeatureId << depends_on.join(", ");
		}
	}
	QMetaObject::invokeMethod(this, "postInstall", Qt::QueuedConnection);
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
	QObject *ret = m_installedPlugins.value(feature_id);
	if(!ret) {
		qfWarning() << "Plugin for feature id:" << feature_id << "is not installed!";
	}
	return ret;
}

Application *MainWindow::application(bool must_exist)
{
	Application *ret = qobject_cast<Application*>(QApplication::instance());
	if(!ret && must_exist) {
		qfFatal("qf::qmlwidgets::framework::Application instance MUST exist.");
	}
	return ret;
}

#include "mainwindow.moc"
