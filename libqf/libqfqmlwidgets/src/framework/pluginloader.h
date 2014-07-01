#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "mainwindow.h"

#include <QObject>

class QQmlComponent;

namespace qf {
namespace qmlwidgets {
namespace framework {

class MainWindow;

class PluginLoader : public QObject
{
	Q_OBJECT
public:
	typedef QMap<QString, QObject*> PluginMap;
public:
	explicit PluginLoader(MainWindow *parent = 0);

	void loadPlugins();
	const PluginMap& loadedPlugins() {return m_loadedPlugins;}

	Q_SIGNAL void loadingFinished();
private:
	MainWindow *mainWindow();

	PluginMap findPlugins();
	void loadPlugins(const PluginMap &manifests_to_load);
	void loadNextPlugin();
	Q_SLOT void continueLoading();
private:
	PluginMap m_loadedPlugins;
	PluginMap m_manifestsToLoad;
	QString m_currentlyLoadedFeatureId;
	QQmlComponent *m_currentlyLoadedComponent;
};

}}}

#endif // PLUGINLOADER_H
