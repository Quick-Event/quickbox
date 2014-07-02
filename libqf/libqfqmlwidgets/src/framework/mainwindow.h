#ifndef QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
#define QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"

#include <QMainWindow>
#include <QMap>

namespace qf {
namespace core {
class Settings;
}
namespace qmlwidgets {

class MenuBar;
class StatusBar;

namespace framework {

class Application;
class PluginLoader;

class QFQMLWIDGETS_DECL_EXPORT MainWindow : public QMainWindow, public IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::MenuBar* menuBar READ menuBar)
	Q_PROPERTY(qf::qmlwidgets::StatusBar* statusBar READ statusBar)
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
private:
	typedef QMainWindow Super;
public:
	typedef QMap<QString, QObject*> PluginMap;
public:
	explicit MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow() Q_DECL_OVERRIDE;
public:
	void loadPlugins();
	/// framework API
	Q_INVOKABLE void setPersistentSettingDomains(const QString &organization_domain, const QString &organization_name, const QString &application_name = QString());
	MenuBar* menuBar();
	StatusBar* statusBar();
	Q_INVOKABLE void setStatusBar(qf::qmlwidgets::StatusBar *sbar);
	Q_INVOKABLE QObject* plugin(const QString &feature_id);

	Q_SLOT void loadPersistentSettings();
	/// emitted by plugin loader when all plugins are loaded
	Q_SIGNAL void pluginsLoaded();

	Q_SIGNAL void logEntry(int level, const QVariantMap &log_entry_map);

	//Q_INVOKABLE QObject* obj_testing();

private:
	Q_SLOT void savePersistentSettings();
	//void setupSettingsPersistence();
private:
	PluginLoader *m_pluginLoader;
	qf::qmlwidgets::StatusBar* m_statusBar;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
