#ifndef QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
#define QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"
//#include "pluginloader.h"

#include <qf/core/utils.h>

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
class ToolBar;

namespace dialogs {
class QmlDialog;
}

namespace framework {

class Application;
class PluginLoader;
class DockWidget;
class PartWidget;
class CentralWidget;
class Plugin;
typedef QList<qf::qmlwidgets::framework::Plugin*> PluginList;

class QFQMLWIDGETS_DECL_EXPORT MainWindow : public QMainWindow, public IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::MenuBar* menuBar READ menuBar)
	Q_PROPERTY(qf::qmlwidgets::StatusBar* statusBar READ statusBar)
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(QString uiLanguageName READ uiLanguageName WRITE setUiLanguageName NOTIFY uiLanguageNameChanged)
private:
	typedef QMainWindow Super;
public:
	explicit MainWindow(QWidget * parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	~MainWindow() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, u, U, iLanguageName)
public:
	CentralWidget* centralWidget();
	void setCentralWidget(CentralWidget *widget);
public:
	void registerPlugin(qf::qmlwidgets::framework::Plugin *plugin);
	virtual void loadPlugins();
	QList<qf::qmlwidgets::framework::Plugin*> installedPlugins();

	/// framework API
	Q_INVOKABLE void setPersistentSettingDomains(const QString &organization_domain, const QString &organization_name, const QString &application_name = QString());

	MenuBar* menuBar();

	Q_INVOKABLE qf::qmlwidgets::ToolBar* toolBar(const QString &name, bool create_if_not_exists = false);

	StatusBar* statusBar();
	Q_INVOKABLE void setStatusBar(qf::qmlwidgets::StatusBar *sbar);

	Q_INVOKABLE void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
	Q_INVOKABLE void addPartWidget(qf::qmlwidgets::framework::PartWidget *widget, const QString &feature_id = QString());

	Q_INVOKABLE qf::qmlwidgets::framework::Plugin* plugin(const QString &feature_id, bool throw_exc = false);
	Q_INVOKABLE qf::qmlwidgets::framework::Plugin* pluginForObject(QObject *qml_object);

	Q_INVOKABLE qf::qmlwidgets::dialogs::QmlDialog* createQmlDialog(QWidget *parent = nullptr);

	Q_SLOT void loadPersistentSettings();

	Q_SLOT void showProgress(const QString &msg, int completed, int total);
	Q_SLOT void hideProgress();
	Q_SIGNAL void progress(const QString &msg, int completed, int total);

	/// emitted by plugin loader when all plugins are loaded
	Q_SIGNAL void pluginsLoaded();
	Q_SIGNAL void applicationLaunched();
	Q_SIGNAL void aboutToClose();
#ifdef GET_RESOURCE_IN_FRAMEWORK
	Q_INVOKABLE int getResource(const QUrl &url, bool show_progress = true);
	Q_SIGNAL void getResourceContentFinished(int request_id, bool ok, const QString &content);
#endif
	static MainWindow* frameWork();

	Q_SLOT bool setActivePart(const QString &feature_id);

	//Q_INVOKABLE QObject* obj_testing();
protected:
	PluginLoader *pluginLoader();
	void closeEvent(QCloseEvent *ev) Q_DECL_OVERRIDE;
	Q_SLOT virtual void onPluginsLoaded();
private:
	Q_SLOT void savePersistentSettings();
private:
	PluginList m_loadedPlugins;
	PluginLoader *m_pluginLoader = nullptr;
	QMap<QString, qf::qmlwidgets::ToolBar*> m_toolBars;
	static MainWindow *self;
};

template<typename T>
static T* getPlugin()
{
	static_assert(std::is_base_of<qf::qmlwidgets::framework::Plugin, T>::value, "given type is not Plugin");
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	T* ret = nullptr;
	for(auto *p : fwk->installedPlugins()) {
		ret = dynamic_cast<T*>(p);
		if(ret)
			break;
	}
	if(!ret) {
		QF_EXCEPTION(QString("Plugin ") + typeid(T).name() + " not installed!");
	}
	return ret;
}

template<typename Widget, typename PartWidget>
static void initPluginWidget(QString title, QString featureId)
{
	auto* widget = new Widget();
	auto *partWidget = new PartWidget(title, featureId);
	qf::qmlwidgets::framework::MainWindow::frameWork()->addPartWidget(partWidget);
	partWidget->addWidget(widget);
	widget->settleDownInPartWidget(partWidget);
}

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
