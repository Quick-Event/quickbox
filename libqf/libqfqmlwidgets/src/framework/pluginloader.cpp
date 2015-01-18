#include "pluginloader.h"
#include "application.h"
#include "mainwindow.h"
#include "plugin.h"
#include "pluginmanifest.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/utils.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QDirIterator>

using namespace qf::qmlwidgets::framework;
/*
const char * PluginLoader::prop_plugin_path = "__pluginPath";

static const char * prop_feature_id = "featureId";
static const char * prop_plugin_main_qml_file = "pluginMainQml";
static const char * prop_depends_on_feature_ids = "dependsOnFeatureIds";
*/
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

void PluginLoader::loadPlugins(const PluginLoader::ManifestMap &manifests_to_load)
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

PluginLoader::ManifestMap PluginLoader::findPlugins()
{
	ManifestMap ret;
	QQmlEngine *qe = Application::instance()->qmlEngine();

	for(auto path : Application::instance()->qmlPluginImportPaths()) {
		qfInfo() << "Finding plugin manifests on:" << path;
		QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::FollowSymlinks);
		while(it.hasNext()) {
			it.next();
			QFileInfo fi = it.fileInfo();
			if(QFile::exists(fi.absoluteFilePath() + "/Manifest.qml")) {
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
						PluginManifest *manifest = qobject_cast<PluginManifest*>(root);
						if(!manifest) {
							qfError() << "Loaded component is not a kind of PluginManifest:" << root << c.url().toString();
							continue;
						}
						QString feature_id = manifest->featureId();
						if(feature_id.isEmpty()) {
							feature_id = fi.baseName();
							//qfInfo() << "FeatureId is empty, setting from plugin name to:" << feature_id;
							/// not sure if QML properties can be set this way, I rathe think that QQmlProperty should be used here
							manifest->setFeatureId(feature_id);
						}
						//QString plugin_loader = "main.qml";
						QStringList depends_on_feature_ids = manifest->dependsOnFeatureIds();
						if(feature_id != CoreFeatureId) {
							/// each not Core feature implicitly depends on Core
							depends_on_feature_ids << CoreFeatureId;
							manifest->setDependsOnFeatureIds(depends_on_feature_ids);
						}

						QString plugin_path = fi.absoluteFilePath();
						manifest->setHomeDir(plugin_path);
						qfInfo() << "Found Manifest.qml for featureId:" << feature_id << "from plugin:" << fi.baseName();
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
								ret[feature_id] = manifest;
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
	QMapIterator<QString, PluginManifest*> it(m_manifestsToLoad);
	while (it.hasNext()) {
		it.next();
		PluginManifest *manifest = it.value();
		QString feature_id = it.key();
		QStringList depends_on = manifest->dependsOnFeatureIds();
		bool dependency_satisfied = true;
		for(auto required_feature_id : depends_on) {
			if(!m_loadedPlugins.contains(required_feature_id)) {
				dependency_satisfied = false;
				break;
			}
		}
		if(dependency_satisfied) {
			QUrl plugin_loader_url = QUrl::fromLocalFile(manifest->homeDir() + "/main.qml");
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
		QMapIterator<QString, PluginManifest*> it(m_manifestsToLoad);
		while (it.hasNext()) {
			it.next();
			PluginManifest *manifest = it.value();
			QString feature_id = it.key();
			QStringList depends_on = manifest->dependsOnFeatureIds();
			qfError() << "\t!!!" << feature_id << "depends on:" << depends_on.join(", ");
		}
	}
	if(m_currentlyLoadedFeatureId.isEmpty()) {
		emit loadingFinished();
	}
}

void PluginLoader::continueLoading()
{
	qfLogFuncFrame() << "feature id:" << m_currentlyLoadedFeatureId << "component:" << m_currentlyLoadedComponent;
	PluginManifest *manifest = m_manifestsToLoad.take(m_currentlyLoadedFeatureId);
	QF_ASSERT(manifest != nullptr, "internal error", return);
	QF_ASSERT(m_currentlyLoadedComponent != nullptr, "internal error", return);

	if(m_currentlyLoadedComponent->isReady()) {
		Application *app = Application::instance();
		app->clearQmlErrorList();

		Plugin *plugin = nullptr;
		QObject *root = m_currentlyLoadedComponent->beginCreate(app->qmlEngine()->rootContext());
		if(root) {
			plugin = qobject_cast<Plugin*>(root);
			if(plugin) {
				//qfInfo() << "set manifest:" << manifest << "to plugin:" << plugin;
				plugin->setManifest(manifest);
			}
			else {
				qfError() << "Loaded component is not a kind of Plugin:" << root << m_currentlyLoadedComponent->url().toString();
			}
			m_currentlyLoadedComponent->completeCreate();
		}
		if(!plugin) {
			qfError() << "Error creating plugin:" << m_currentlyLoadedComponent->url().toString();
			qfError() << m_currentlyLoadedComponent->errorString();
		}
		else {
			plugin->setParent(mainWindow());
			m_loadedPlugins[m_currentlyLoadedFeatureId] = plugin;
		}

		if(app->qmlErrorList().count()) {
			qfError() << "Feature:" << m_currentlyLoadedFeatureId << "install ERROR";
		}
		else {
			qfInfo() << "Feature:" << m_currentlyLoadedFeatureId << "install SUCCESS";
			emit plugin->installed();
		}
	}
	else {
		qfError() << "Status:" << m_currentlyLoadedComponent->status() << "error:" << m_currentlyLoadedComponent->errorString();
	}
	//manifest->deleteLater();
	QF_SAFE_DELETE(m_currentlyLoadedComponent);
	QMetaObject::invokeMethod(this, "loadNextPlugin", Qt::QueuedConnection);
}
