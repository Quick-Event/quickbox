#include "mainwindow.h"
#include "application.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QtQml>

int main(int argc, char *argv[])
{
	QScopedPointer<qf::core::LogDevice> log_device(qf::core::FileLogDevice::install(argc, argv));
	log_device->setPrettyDomain(true);

	qfError() << "QFLog(ERROR) test OK.";
	qfWarning() << "QFLog(WARNING) test OK.";
	qfInfo() << "QFLog(INFO) test OK.";
	qfDebug() << "QFLog(DEBUG) test OK.";

	qDebug() << "creating application instance";
	//qFatal("ASSERT");

	Application app(argc, argv);

	MainWindow main_window;
	main_window.loadPlugins();
	main_window.show();

	int ret = app.exec();

	return ret;
}
