#include "application.h"
#include "appclioptions.h"
#include "mainwindow.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QSettings>

int main(int argc, char *argv[])
{
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setOrganizationName("quickbox");
	QCoreApplication::setApplicationName("quickshow");

	QStringList args = qf::core::LogDevice::setGlobalTresholds(argc, argv);
	QScopedPointer<qf::core::LogDevice> log_device(qf::core::FileLogDevice::install());
	AppCliOptions cli_opts;

	cli_opts.parse(args);
	if(cli_opts.isParseError()) {
		foreach(QString err, cli_opts.parseErrors())
			qfError() << err;
		return 1;
	}
	if(cli_opts.isAppBreak()) {
		if(cli_opts.isHelp())
			cli_opts.printHelp();
		return 0;
	}
	foreach(QString s, cli_opts.unusedArguments()) {
		qDebug() << "Undefined argument:" << s;
	}

	// Uncaught exception is intentional here
	if(!cli_opts.loadConfigFile())
		return 1;

	Application app(argc, argv, &cli_opts);

	MainWindow w;
	w.show();

	return app.exec();
}
