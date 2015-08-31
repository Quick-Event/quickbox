#include "mainwindow.h"
#include "application.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>
#include <qf/core/utils/settings.h>

#include <QtQml>
#include <QLocale>

#include <iostream>


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("QuickBox");
	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setApplicationName("QuickEvent");

	QString o_log_file;
	for (int i = 1; i < argc-1; ++i) {
		if(argv[i] == QLatin1String("--log-file"))
			o_log_file = argv[i + 1];
	}

	QScopedPointer<qf::core::FileLogDevice> file_log_device(qf::core::FileLogDevice::install());
	file_log_device->setFile(o_log_file);
	file_log_device->setDomainTresholds(argc, argv);
	file_log_device->setPrettyDomain(true);

	qfError() << "QFLog(ERROR) test OK.";// << QVariant::typeToName(QVariant::Int) << QVariant::typeToName(QVariant::String);
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
	QString lc_name;
	{
		qf::core::utils::Settings settings;
		lc_name = settings.value(MainWindow::SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE).toString();
		if(lc_name.isEmpty() || lc_name == QLatin1String("system"))
			lc_name = QLocale::system().name();
		qfInfo() << "Loading translations for:" << lc_name;
		{
			QTranslator *qt_translator = new QTranslator(&app);
			QString tr_name = "qt_" + lc_name;
			bool ok = qt_translator->load(tr_name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
			if(ok) {
				qfInfo() << "Installing translator for:" << tr_name;
				app.installTranslator(qt_translator);
			}
			else {
				qfInfo() << "Erorr loading translator:" << tr_name << "from:" << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
			}
		}
		for(QString prefix : {"libqfcore", "libqfqmlwidgets", "quickevent"}) {
			QTranslator *qt_translator = new QTranslator(&app);
			QString tr_name = prefix + "." + lc_name;
			//qfInfo() << "##" << QCoreApplication::applicationDirPath();
			bool ok = qt_translator->load(tr_name, QCoreApplication::applicationDirPath());
			if(ok) {
				qfInfo() << "Installing translator for:" << tr_name;
				app.installTranslator(qt_translator);
			}
			else {
				qfInfo() << "Erorr loading translator:" << tr_name << "in:" << QCoreApplication::applicationDirPath();
			}
		}
	}
	MainWindow main_window;
	main_window.setUiLanguageName(lc_name);
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
