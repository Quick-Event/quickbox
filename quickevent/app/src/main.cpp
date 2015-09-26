#include "mainwindow.h"
#include "application.h"
#include "appclioptions.h"

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
	QStringList args = file_log_device->setDomainTresholds(argc, argv);

	qfError() << "QFLog(ERROR) test OK.";// << QVariant::typeToName(QVariant::Int) << QVariant::typeToName(QVariant::String);
	qfWarning() << "QFLog(WARNING) test OK.";
	qfInfo() << "QFLog(INFO) test OK.";
	qfDebug() << "QFLog(DEBUG) test OK.";

	AppCliOptions cli_opts;
	cli_opts.parse(args);
	if(cli_opts.isParseError()) {
		foreach(QString err, cli_opts.parseErrors())
			qfError() << err;
		return 1;
	}
	if(cli_opts.isAppBreak()) {
		if(cli_opts.isHelp())
			std::cout << qf::core::LogDevice::dCommandLineSwitchHelp();
		return 0;
	}
	foreach(QString s, cli_opts.unusedArguments()) {
		qfWarning() << "Undefined argument:" << s;
	}

	if(!cli_opts.loadConfigFile())
		return 1;

	qDebug() << "creating application instance";
	Application app(argc, argv, &cli_opts);

	QString lc_name;
	{
		if(cli_opts.locale_isset()) {
			lc_name = cli_opts.locale();
		}
		else {
			qf::core::utils::Settings settings;
			lc_name = settings.value(MainWindow::SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE).toString();
		}
		if(lc_name.isEmpty() || lc_name == QLatin1String("system"))
			lc_name = QLocale::system().name();
		QString app_translations_path = QCoreApplication::applicationDirPath() + "/translations";
		QString qt_translations_path = app_translations_path;
		qfInfo() << "Loading translations for:" << lc_name;
		{
			QTranslator *qt_translator = new QTranslator(&app);
			QString tr_name = "qt_" + lc_name;
			bool ok = qt_translator->load(tr_name, qt_translations_path);
			if(ok) {
				ok = app.installTranslator(qt_translator);
				qfInfo() << "Installing translator file:" << tr_name << " ... " << (ok? "OK": "ERROR");
			}
			else {
				qfInfo() << "Erorr loading translator file:" << (qt_translations_path + '/' + tr_name);
			}
		}
		for(QString prefix : {"libqfcore", "libqfqmlwidgets", "libsiut", "quickevent"}) {
			QTranslator *qt_translator = new QTranslator(&app);
			QString tr_name = prefix + "." + lc_name;
			bool ok = qt_translator->load(tr_name, app_translations_path);
			if(ok) {
				ok = app.installTranslator(qt_translator);
				qfInfo() << "Installing translator file:" << tr_name << " ... " << (ok? "OK": "ERROR");
			}
			else {
				qfInfo() << "Erorr loading translator file:" << (app_translations_path + '/' + tr_name);
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
