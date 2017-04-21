#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qf/qmlwidgets/framework/mainwindow.h>

class MainWindow : public qf::qmlwidgets::framework::MainWindow
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::MainWindow Super;
public:
	explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow();

	Q_INVOKABLE QString versionString() const;

	static const QLatin1String SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE;
	Q_INVOKABLE QString settingsPrefix_application_locale_language();
protected:
	void onPluginsLoaded() Q_DECL_OVERRIDE;
};

#endif // MAINWINDOW_H
