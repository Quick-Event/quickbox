#include "mainwindow.h"
#include "application.h"
#include "../menubar.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
//#include <qf/core/string.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QDirIterator>
#include <QQmlComponent>
#include <QSettings>

using namespace qf::qmlwidgets::framework;

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
		QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks);
		while(it.hasNext()) {
			it.next();
			QFileInfo fi = it.fileInfo();
			if(QFile::exists(fi.absoluteFilePath() + "/qmldir")) {
				qfDebug() << "Trying to load plugin on path:" << fi.absoluteFilePath();
				QQmlComponent c(qe, QUrl::fromLocalFile(fi.absoluteFilePath() + "/main.qml"));
				if(!c.isReady()) {
					qfError() << c.errorString();
				}
				else {
					QObject *root = c.create();
					if(!root) {
						qfError() << "Error creating plugin:" << c.url().toString();
						qfError() << c.errorString();
					}
					else {
						QString feature_id = root->property("featureId").toString();
						if(feature_id.isEmpty()) {
							qfError() << "Error creating plugin: featureId is empty.";
							delete root;
						}
						else {
							qfInfo() << "Importing featureId:" << feature_id;
							if(ret.contains(feature_id)) {
								qfError() << "Feature id:" << feature_id << "already loaded";
							}
							else {
								root->setParent(this);
								ret[feature_id] = root;
								/*
								QString feature_slot = root->property("featureSlot").toString();
								if(feature_slot.isEmpty())
									feature_slot = "others";
								features_by_slot[feature_slot] << feature_id;
								if(feature_id == "Core") {
									m_featureSlots = root->property("featureSlots").toStringList();
								}
								*/
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

static const QLatin1String CoreFeatureId("Core");

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
			QObject *plugin = it.value();
			QString feature_id = it.key();
		    QStringList depends_on = plugin->property("dependsOnFeatureIds").toStringList();
			if(feature_id != CoreFeatureId) {
				// each not Core feature implicitly depends on Core
				depends_on << CoreFeatureId;
			}
			bool dependency_satisfied = true;
			for(auto required_feature_id : depends_on) {
				if(!m_installedPlugins.contains(required_feature_id)) {
					dependency_satisfied = false;
					break;
				}
			}
			if(dependency_satisfied) {
				qfInfo() << "Installing feature:" << feature_id;
				Application *app = application();
				app->clearQmlErrorList();
				QMetaObject::invokeMethod(plugin, "install");
				if(app->qmlErrorList().count()) {
					qfError() << "Feature:" << feature_id << "install ERROR.";
				}
				else {
					qfInfo() << "Feature:" << feature_id << "install SUCCESS.";
					m_installedPlugins[feature_id] = plugin;
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
