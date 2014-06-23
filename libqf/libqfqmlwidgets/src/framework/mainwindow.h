#ifndef QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
#define QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"

#include <QMainWindow>
#include <QMap>

namespace qf {
namespace qmlwidgets {
//class Menu;
class MenuBar;
namespace framework {

class Application;

class QFQMLWIDGETS_DECL_EXPORT MainWindow : public QMainWindow, public IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::MenuBar* menuBar READ menuBar)
private:
	typedef QMainWindow Super;
public:
	typedef QMap<QString, QObject*> PluginMap;
public:
	explicit MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow() Q_DECL_OVERRIDE;
public:
	virtual void loadPlugins();
private:
	Application* application(bool must_exist = true);
protected:
	virtual PluginMap findPlugins();
	virtual void installPlugins(const PluginMap &plugins_to_install);
public:
	Q_SLOT void setPersistentSettingsKey(const QString &k) {IPersistentSettings::setPersistentSettingsKey(k);}
private:
	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
	//void setupSettingsPersistence();
public slots:
	/// framework API
	MenuBar* menuBar();
	QObject* plugin(const QString &feature_id);
private:
	PluginMap m_installedPlugins;
	QStringList m_featureSlots;
};

}
}
}

#endif // QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
