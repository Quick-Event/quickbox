#include "mainwindow.h"
#include "application.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QtQml>
#include <QLocale>

#include <iostream>

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

	bool o_help = false;

	QStringList args = app.arguments();
	for(int i=0; i<args.count(); i++) {
		QString arg = args.value(i);
		if(arg == QLatin1String("--help") || arg == QLatin1String("-h")) {
			o_help = true;
		}
	}

	if(o_help) {
		std::cout << args.value(0).toStdString() << " [OPTIONS]" << std::endl << std::endl;
		std::cout << "OPTIONS:" << std::endl;
		std::cout << "--profile <path_to_profile_file>\tloads specific profile (see: https://github.com/fvacek/quickbox/wiki/Application-profiles)" << std::endl;
		std::cout << "-d[domain[:LEVEL]]\tset debug domain and level" << std::endl;
		std::cout << "\t\t\tdomain: any substring of source module, for example 'mymod' prints debug info from every source file with name containing 'mymod', mymodule.cpp, tomymod.cpp, ..." << std::endl;
		std::cout << "\t\t\tLEVEL: any of DEB, INFO, WARN, ERR, default level is INFO" << std::endl;
		return 0;
	}

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
