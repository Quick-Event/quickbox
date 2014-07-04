#ifndef QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
#define QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"
//#include "centralwidget.h"

#include <QMainWindow>
#include <QMap>

namespace qf {
namespace core {
class Settings;
}
}
namespace qf {
namespace qmlwidgets {

class MenuBar;
class StatusBar;

namespace framework {

class Application;
class PluginLoader;
class DockWidget;
class PartWidget;
class CentralWidget;

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
	CentralWidget* centralWidget();
	void setCentralWidget(CentralWidget *widget);
public:
	void loadPlugins();
	/// framework API
	Q_INVOKABLE void setPersistentSettingDomains(const QString &organization_domain, const QString &organization_name, const QString &application_name = QString());

	MenuBar* menuBar();

	StatusBar* statusBar();
	Q_INVOKABLE void setStatusBar(qf::qmlwidgets::StatusBar *sbar);

	Q_INVOKABLE void addDockWidget(Qt::DockWidgetArea area, qf::qmlwidgets::framework::DockWidget *dockwidget);
	Q_INVOKABLE void addPartWidget(qf::qmlwidgets::framework::PartWidget *widget);

	Q_INVOKABLE QObject* plugin(const QString &feature_id);

	Q_SLOT void loadPersistentSettings();

	/// emitted by plugin loader when all plugins are loaded
	Q_SIGNAL void pluginsLoaded();
	Q_SIGNAL void aboutToClose();

	//Q_SIGNAL void logEntry(int level, const QVariantMap &log_entry_map);

	//Q_INVOKABLE QObject* obj_testing();
protected:
	void closeEvent(QCloseEvent *ev) Q_DECL_OVERRIDE;
private:
	Q_SLOT void savePersistentSettings();
	//void setupSettingsPersistence();
private:
	PluginLoader *m_pluginLoader;
	qf::qmlwidgets::StatusBar* m_statusBar;
	//CentralWidget *m_centralWidget;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
