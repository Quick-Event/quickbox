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

	void loadPlugins();
	const PluginMap& loadedPlugins() {return m_loadedPlugins;}

	Q_SIGNAL void loadingFinished();
private:
	MainWindow *mainWindow();

	ManifestMap findPlugins();
	void loadPlugins(const ManifestMap &manifests_to_load);
	Q_SLOT void loadNextPlugin();
	Q_SLOT void continueLoading();
private:
	PluginMap m_loadedPlugins;
	ManifestMap m_manifestsToLoad;
	QString m_currentlyLoadedFeatureId;
	QQmlComponent *m_currentlyLoadedComponent;
};

}}}

#endif // PLUGINLOADER_H
