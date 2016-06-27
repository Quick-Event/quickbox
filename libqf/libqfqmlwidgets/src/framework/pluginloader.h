#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "mainwindow.h"

#include <QObject>

class QQmlComponent;

namespace qf {
namespace qmlwidgets {
namespace framework {

class MainWindow;
class PluginManifest;

class PluginLoader : public QObject
{
	Q_OBJECT
public:
	typedef QMap<QString, qf::qmlwidgets::framework::Plugin*> PluginMap;
	typedef QMap<QString, qf::qmlwidgets::framework::PluginManifest*> ManifestMap;
public:
	explicit PluginLoader(MainWindow *parent = 0);

	void loadPlugins(const QStringList &feature_ids = QStringList());
	const PluginMap& loadedPlugins() {return m_loadedPlugins;}

	Q_SIGNAL void loadingFinished();
private:
	MainWindow *mainWindow();

	ManifestMap findPlugins();
	bool loadPlugin(const QString feature_id);
	bool loadPluginComponent(QQmlComponent *plugin_component, PluginManifest *manifest);

	//Q_SLOT void loadNextPlugin();
	//Q_SLOT void continueLoading();
private:
	PluginMap m_loadedPlugins;
	ManifestMap m_manifestsToLoad;
	//QString m_currentlyLoadedFeatureId;
	//QQmlComponent *m_currentlyLoadedComponent;
};

}}}

#endif // PLUGINLOADER_H
