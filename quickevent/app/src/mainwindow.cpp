#include "mainwindow.h"
#include "application.h"
#include "loggerwidget.h"
#include "tablemodellogdevice.h"

#include <qf/qmlwidgets/framework/stackedcentralwidget.h>
#include <qf/qmlwidgets/framework/partswitch.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/action.h>

#include <QLabel>

const QLatin1String MainWindow::SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE("application/locale/language");

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	Super(parent, flags)
{
	setWindowTitle(tr("Quick Event ver. %1").arg(versionString()));
	setWindowIcon(QIcon(":/quickevent/images/quickevent64.png"));
}

MainWindow::~MainWindow()
{
}

QString MainWindow::versionString() const
{
	static QString s = QLatin1String("1.1.7");
	return s;
}

QString MainWindow::settingsPrefix_application_locale_language()
{
	static const QString s = SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE;
	return s;
}

void MainWindow::onPluginsLoaded()
{
	{
		auto *dw = new qf::qmlwidgets::framework::DockWidget(nullptr);
		dw->setObjectName("loggingDockWidget");
		dw->setWindowTitle(tr("Application log"));
		addDockWidget(Qt::BottomDockWidgetArea, dw);
		auto *w = new LoggerWidget();
		connect(dw, &qf::qmlwidgets::framework::DockWidget::visibilityChanged, w, &LoggerWidget::onDockWidgetVisibleChanged);
		dw->setWidget(w);
		dw->hide();
		auto *a = dw->toggleViewAction();
		//a->setCheckable(true);
		a->setShortcut(QKeySequence("ctrl+shift+L"));
		menuBar()->actionForPath("view")->addActionInto(a);
	}

	auto *w = qobject_cast<qf::qmlwidgets::framework::StackedCentralWidget*>(centralWidget());
	menuBar()->actionForPath("view/toolbar")->addActionInto(w->partSwitch()->toggleViewAction());

	centralWidget()->setActivePart("Competitors", true);
	setPersistentSettingsId("MainWindow");
	loadPersistentSettings();
}

