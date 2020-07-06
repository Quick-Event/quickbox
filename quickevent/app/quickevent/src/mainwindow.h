#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qf/qmlwidgets/framework/mainwindow.h>

class MainWindow : public qf::qmlwidgets::framework::MainWindow
{
	Q_OBJECT
private:
	using Super = qf::qmlwidgets::framework::MainWindow;
public:
	explicit MainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	~MainWindow() override;

	Q_INVOKABLE QString versionString() const;
	Q_INVOKABLE QString dbVersionString() const;
	Q_INVOKABLE int dbVersion() const;

	void loadPlugins() override;
protected:
	void onPluginsLoaded() Q_DECL_OVERRIDE;
};

#endif // MAINWINDOW_H
