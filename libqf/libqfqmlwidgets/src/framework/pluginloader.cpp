#include "pluginloader.h"
#include "application.h"
#include "mainwindow.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/utils.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QDirIterator>

using namespace qf::qmlwidgets::framework;

static const char * prop_feature_id = "featureId";
static const char * prop_plugin_loader = "pluginLoader";
static const char * prop_plugin_path = "__pluginPath";
static const char * prop_depends_on_feature_ids = "dependsOnFeatureIds";
static const QLatin1String CoreFeatureId("Core");

PluginLoader::PluginLoader(MainWindow *parent) :
	QObject(parent)
{
}

void PluginLoader::loadPlugins()
{
	qfLogFuncFrame();
	loadPlugins(findPlugins());
}

void PluginLoader::loadPlugins(const PluginLoader::PluginMap &manifests_to_load)
{
	m_manifestsToLoad = manifests_to_load;
	loadNextPlugin();
}

MainWindow *PluginLoader::mainWindow()
{
	MainWindow *ret = qobject_cast<MainWindow*>(this->parent());
	Q_ASSERT(ret != nullptr);
	return ret;
}

PluginLoader::PluginMap PluginLoader::findPlugins()
{
	PluginMap ret;
	QQmlEngine *qe = Application::instance()->qmlEngine();

	for(auto path : Application::instance()->qmlPluginImportPaths()) {
		qfInfo() << "Finding plugin manifests on:" << path;
		QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks);
		while(it.hasNext()) {
			it.next();
			QFileInfo fi = it.fileInfo();
			if(QFile::exists(fi.absoluteFilePath() + "/qmldir")) {
				qfDebug() << "Trying to load plugin manifest on path:" << (fi.absoluteFilePath() + "/Manifest.qml");
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
							/// each not Core feature implicitly depends on Core
							depends_on_feature_ids << CoreFeatureId;
							root->setProperty(prop_depends_on_feature_ids, depends_on_feature_ids);
						}

						QString plugin_path = fi.absoluteFilePath();
						root->setProperty(prop_plugin_path, plugin_path);
						qfInfo() << "Found Manifest.qml for featureId:" << feature_id << "from plugin:" << fi.baseName() << "loader:" << plugin_loader;
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

void PluginLoader::loadNextPlugin()
{
	QQmlEngine *qe = Application::instance()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return);

	m_currentlyLoadedFeatureId = QString();
	QMapIterator<QString, QObject*> it(m_manifestsToLoad);
	while (it.hasNext()) {
	    it.next();
		QObject *manifest = it.value();
		QString feature_id = it.key();
	    QStringList depends_on = manifest->property(prop_depends_on_feature_ids).toStringList();
		bool dependency_satisfied = true;
		for(auto required_feature_id : depends_on) {
			if(!m_loadedPlugins.contains(required_feature_id)) {
				dependency_satisfied = false;
				break;
			}
		}
		if(dependency_satisfied) {
			QUrl plugin_loader_url = QUrl::fromLocalFile(manifest->property(prop_plugin_path).toString()
														 + "/"
														 + manifest->property(prop_plugin_loader).toString());
			qfInfo() << "Installing feature:" << feature_id << "from:" << plugin_loader_url.toString();
			m_currentlyLoadedFeatureId = feature_id;
			m_currentlyLoadedComponent = new QQmlComponent(qe, plugin_loader_url, QQmlComponent::PreferSynchronous);
			if(m_currentlyLoadedComponent->isLoading()) {
				QObject::connect(m_currentlyLoadedComponent, &QQmlComponent::statusChanged, this, &PluginLoader::continueLoading);
			}
			else {
				continueLoading();
			}
			break;
		}
	}
	if(!m_manifestsToLoad.isEmpty() && m_currentlyLoadedFeatureId.isEmpty()) {
		qfError() << "Features not installed due to unsatisfied dependeces:";
		QMapIterator<QString, QObject*> it(m_manifestsToLoad);
		while (it.hasNext()) {
		    it.next();
			QObject *plugin = it.value();
			QString feature_id = it.key();
		    QStringList depends_on = plugin->property("dependsOnFeatureIds").toStringList();
			qfError() << "\t!!!" << feature_id << "depends on:" << CoreFeatureId << depends_on.join(", ");
		}
	}
	if(m_currentlyLoadedFeatureId.isEmpty()) {
		emit loadingFinished();
	}
}

void PluginLoader::continueLoading()
{
	qfLogFuncFrame() << "feature id:" << m_currentlyLoadedFeatureId << "component:" << m_currentlyLoadedComponent;
	QObject *manifest = m_manifestsToLoad.take(m_currentlyLoadedFeatureId);
	QF_ASSERT(manifest != nullptr, "internal error", return);
	QF_ASSERT(m_currentlyLoadedComponent != nullptr, "internal error", return);

	if(m_currentlyLoadedComponent->isReady()) {
		Application *app = Application::instance();
		app->clearQmlErrorList();

		QObject *plugin = m_currentlyLoadedComponent->create();
		if(!plugin) {
			qfError() << "Error creating plugin:" << m_currentlyLoadedComponent->url().toString();
			qfError() << m_currentlyLoadedComponent->errorString();
		}
		else {
			QVariant v = QVariant::fromValue(manifest);
			plugin->setProperty("manifest", v);
			plugin->setParent(mainWindow());
			m_loadedPlugins[m_currentlyLoadedFeatureId] = plugin;
		}

		if(app->qmlErrorList().count()) {
			qfError() << "Feature:" << m_currentlyLoadedFeatureId << "install ERROR.";
		}
		else {
			qfInfo() << "Feature:" << m_currentlyLoadedFeatureId << "install SUCCESS.";
		}
	}
	else {
		qfError() << "Status:" << m_currentlyLoadedComponent->status() << "error:" << m_currentlyLoadedComponent->errorString();
	}
	manifest->deleteLater();
	QF_SAFE_DELETE(m_currentlyLoadedComponent);
	QMetaObject::invokeMethod(this, "loadNextPlugin", Qt::QueuedConnection);
}
