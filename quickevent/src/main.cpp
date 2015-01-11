#include "mainwindow.h"
#include "application.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QtQml>
#include <QLocale>
//#include <qqml.h>

int main(int argc, char *argv[])
{
	QScopedPointer<qf::core::LogDevice> file_log_device(qf::core::FileLogDevice::install());
	file_log_device->setDomainTresholds(argc, argv);
	file_log_device->setPrettyDomain(true);

	qfError() << "QFLog(ERROR) test OK.";
	qfWarning() << "QFLog(WARNING) test OK.";
	qfInfo() << "QFLog(INFO) test OK.";
	qfDebug() << "QFLog(DEBUG) test OK.";

	//QLocale::setDefault(QLocale(QLocale::Czech));

	qDebug() << "creating application instance";
	Application app(argc, argv);

	MainWindow main_window;
	//QObject::connect(signal_log_device.data(), &qf::core::SignalLogDevice::logEntry, &main_window, &MainWindow::logEntry, Qt::QueuedConnection);
	/*
	QObject::connect(signal_log_device.data(), SIGNAL(logEntry(int, QVariantMap)),
					 &main_window, SIGNAL(logEntry(int, QVariantMap)),
					 Qt::QueuedConnection);
	*/
	main_window.loadPlugins();
	main_window.show();

	int ret = app.exec();

	return ret;
}
