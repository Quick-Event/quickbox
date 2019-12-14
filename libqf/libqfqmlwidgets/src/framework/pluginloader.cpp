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
#include <QTranslator>

using namespace qf::qmlwidgets::framework;

PluginLoader::PluginLoader(MainWindow *parent) :
	QObject(parent)
{
}

PluginLoader::ManifestMap PluginLoader::findPlugins()
{
	ManifestMap ret;
	QQmlEngine *qe = Application::instance()->qmlEngine();

	Q_FOREACH(auto path, Application::instance()->qmlPluginImportPaths()) {
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
						manifest->setDependsOnFeatureIds(depends_on_feature_ids);

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

void PluginLoader::loadPlugins(const QStringList &feature_ids)
{
	qfLogFuncFrame();
	m_manifestsToLoad = findPlugins();
	QStringList fids = feature_ids;
	if(fids.isEmpty())
		Q_FOREACH(auto m, m_manifestsToLoad)
			fids << m->featureId();
	Q_FOREACH(auto feature_id, fids)
		loadPlugin(feature_id);
	emit loadingFinished();
}

bool PluginLoader::loadPlugin(const QString feature_id)
{
	if(m_loadedPlugins.contains(feature_id)) {
		// feature installed already
		return true;
	}

	PluginManifest *manifest = m_manifestsToLoad.take(feature_id);
	if(manifest == nullptr) {
		qfError() << QString("Feature id: '%1' - Invalid manifest!").arg(feature_id);
		return false;
	}

	QQmlEngine *qe = Application::instance()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return false);

	bool ok = true;
	QStringList depends_on = manifest->dependsOnFeatureIds();
	Q_FOREACH(auto required_feature_id, depends_on) {
		if(!m_loadedPlugins.contains(required_feature_id)) {
			qfInfo() << feature_id << "solving dependency on" << required_feature_id;
			ok = loadPlugin(required_feature_id);
			if(!ok) {
				qfError() << "Cannot load feature:" << feature_id << "due to unsatisfied dependecies!";
				qfError() << "\t!!!" << feature_id << "depends on:" << depends_on.join(", ");
				break;
			}
		}
		else {
			qfInfo() << feature_id << "dependency on" << required_feature_id << "satisfied";
		}
	}
	if(ok) {
		QUrl plugin_loader_url = QUrl::fromLocalFile(manifest->homeDir() + "/main.qml");
		qfInfo() << "Installing feature:" << feature_id << "from:" << plugin_loader_url.toString();
		QString lc_name = mainWindow()->uiLanguageName();
		if(!lc_name.isEmpty()) {
			QString tr_name = feature_id + '.' + lc_name;
			QString app_translations_path = QCoreApplication::applicationDirPath() + "/translations";
			QTranslator *trans = new QTranslator(mainWindow());
			bool ok = trans->load(tr_name, app_translations_path);
			if(ok) {
				qfInfo() << "Found translation file for:" << tr_name;
				QCoreApplication::instance()->installTranslator(trans);
			}
			else {
				qfInfo() << "Cannot load translation file for:" << tr_name << "in:" << app_translations_path;
				delete trans;
			}
		}
		QQmlComponent *c = new QQmlComponent(qe, plugin_loader_url, QQmlComponent::PreferSynchronous);
		if(c->isLoading()) {
			qfError() << "Asynchronous plugin loading is not supported!";
			ok = false;
			//QObject::connect(m_currentlyLoadedComponent, &QQmlComponent::statusChanged, this, &PluginLoader::continueLoading);
		}
		else if(!c->isReady()) {
			qfError() << "Component is not ready!" << c->errorString();
		}
		else {
			ok = loadPluginComponent(c, manifest);
		}
	}
	if(!ok) {
		QF_SAFE_DELETE(manifest);
		qfError() << "ERROR load feature:" << feature_id;
	}
	return ok;
}

bool PluginLoader::loadPluginComponent(QQmlComponent *plugin_component, PluginManifest *manifest)
{
	Application *app = Application::instance();
	app->clearQmlErrorList();

	Plugin *plugin = nullptr;
	QObject *root = plugin_component->beginCreate(app->qmlEngine()->rootContext());
	if(root) {
		plugin = qobject_cast<Plugin*>(root);
		if(plugin) {
			//qfInfo() << "set manifest:" << manifest << "to plugin:" << plugin;
			plugin->setManifest(manifest);
		}
		else {
			qfError() << "Loaded component is not a kind of Plugin:" << root << plugin_component->url().toString();
		}
		plugin_component->completeCreate();
	}
	bool ret = (plugin != nullptr);
	if(!plugin) {
		qfError() << "Error creating plugin:" << plugin_component->url().toString();
		qfError() << plugin_component->errorString();
	}
	else {
		plugin->setParent(mainWindow());
		m_loadedPlugins[manifest->featureId()] = plugin;
	}

	if(!ret) {
		qfError() << "Feature:" << manifest->featureId() << "install ERROR";
	}
	else {
		qfInfo() << "Feature:" << manifest->featureId() << "install SUCCESS";
		emit plugin->installed();
	}
	return ret;
}

MainWindow *PluginLoader::mainWindow()
{
	MainWindow *ret = qobject_cast<MainWindow*>(this->parent());
	Q_ASSERT(ret != nullptr);
	return ret;
}

