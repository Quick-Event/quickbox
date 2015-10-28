#include "mainwindow.h"

#include <qf/qmlwidgets/framework/centralwidget.h>

#include <QLabel>

const QLatin1String MainWindow::SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE("application/locale/language");

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	Super(parent, flags)
{
	setWindowTitle(tr("Quick Event ver. %1").arg(versionString()));
}

MainWindow::~MainWindow()
{
}

QString MainWindow::versionString()
{
	static QString s = QLatin1String("0.1.3");
	return s;
}

QString MainWindow::settingsPrefix_application_locale_language()
{
	static const QString s = SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE;
	return s;
}

void MainWindow::whenPluginsLoaded()
{
	centralWidget()->setActivePart("Competitors", true);
}
