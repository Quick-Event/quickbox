#ifndef QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
#define QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H

#include "../qmlwidgetsglobal.h"

#include <QMainWindow>
#include <QMap>

namespace qf {
namespace qmlwidgets {
class Menu;
namespace framework {

class Application;

class QFQMLWIDGETS_DECL_EXPORT MainWindow : public QMainWindow
{
	Q_OBJECT
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
public slots:
	/// framework API
	Menu* menuOnPath(const QString &path);
private:
	PluginMap m_installedPlugins;
	QStringList m_featureSlots;
};

}
}
}

#endif // QF_QMLWIDGETS_FRAMEWORK_MAINWINDOW_H
