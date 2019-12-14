#include "mainwindow.h"
#include "application.h"
#include "appversion.h"
#include "appclioptions.h"
#include "tablemodellogdevice.h"

#include <Core/coreplugin.h>

#include <quickevent/core/si/siid.h>
#include <quickevent/core/og/timems.h>

#include <qf/core/log.h>
#include <qf/core/logdevice.h>
#include <qf/core/utils/settings.h>
#include <qf/core/model/logtablemodel.h>

#include <QtQml>
#include <QLocale>

#include <iostream>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("quickbox");
	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setApplicationName("quickevent");
	QCoreApplication::setApplicationVersion(APP_VERSION);

	QSettings::setDefaultFormat(QSettings::IniFormat);

	QString o_log_file;
	for (int i = 1; i < argc-1; ++i) {
		if(argv[i] == QLatin1String("--log-file"))
			o_log_file = argv[i + 1];
	}
	if(o_log_file.isEmpty())
		o_log_file = QDir::tempPath() + "/quickevent.log";

	QStringList args = qf::core::LogDevice::setGlobalTresholds(argc, argv);
	QScopedPointer<qf::core::FileLogDevice> stderr_log_device(qf::core::FileLogDevice::install());
	QScopedPointer<qf::core::FileLogDevice> file_log_device(qf::core::FileLogDevice::install());
	file_log_device->setFile(o_log_file);

	QScopedPointer<TableModelLogDevice> table_model_log_device(TableModelLogDevice::install());
	table_model_log_device->setObjectName(TABLE_MODEL_LOG_DEVICE);

	qfError() << "QFLog(ERROR) test OK.";// << QVariant::typeToName(QVariant::Int) << QVariant::typeToName(QVariant::String);
	qfWarning() << "QFLog(WARNING) test OK.";
	qfInfo() << "QFLog(INFO) test OK.";
	qfDebug() << "QFLog(DEBUG) test OK.";

	qfInfo() << "========================================================";
	qfInfo() << QDateTime::currentDateTime().toString(Qt::ISODate) << "starting" << QCoreApplication::applicationName() << "ver:" << QCoreApplication::applicationVersion();
	qfInfo() << "Log file:" << o_log_file;
	qfInfo() << "========================================================";

	quickevent::core::og::TimeMs::registerQVariantFunctions();
	quickevent::core::si::SiId::registerQVariantFunctions();

	AppCliOptions cli_opts;
	cli_opts.parse(args);
	cli_opts.parse(args);
	if(cli_opts.isParseError()) {
		foreach(QString err, cli_opts.parseErrors())
			qfError() << err;
		return EXIT_FAILURE;
	}
	if(cli_opts.isAppBreak()) {
		if(cli_opts.isHelp())
			cli_opts.printHelp();
		return EXIT_SUCCESS;
	}
	foreach(QString s, cli_opts.unusedArguments()) {
		qDebug() << "Undefined argument:" << s;
	}

    // Uncaught exception is intentional here
	if(!cli_opts.loadConfigFile()) {
		return EXIT_FAILURE;
	}

	qDebug() << "creating application instance";
	Application app(argc, argv, &cli_opts);

	QString lc_name;
	{
		if(cli_opts.locale_isset()) {
			lc_name = cli_opts.locale();
		}
		else {
			qf::core::utils::Settings settings;
			lc_name = settings.value(Core::Plugin::SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE()).toString();
		}
		if(lc_name.isEmpty() || lc_name == QLatin1String("system"))
			lc_name = QLocale::system().name();
		QString app_translations_path = QCoreApplication::applicationDirPath() + "/translations";
		qfInfo() << "Loading translations for:" << lc_name;
		{
			QTranslator *qt_translator = new QTranslator(&app);
			QString tr_name = "qt_" + lc_name;
			bool ok = qt_translator->load(tr_name, app_translations_path);
			if(ok) {
				ok = app.installTranslator(qt_translator);
				qfInfo() << "Installing translator file:" << tr_name << " ... " << (ok? "OK": "ERROR");
			}
			else {
				qfInfo() << "Erorr loading translator file:" << (app_translations_path + '/' + tr_name);
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
