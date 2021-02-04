#include "application.h"
#include "appclioptions.h"

#include <qf/core/log.h>
#include <necrolog.h>

#include <QSettings>

int main(int argc, char *argv[])
{
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setOrganizationName("quickbox");
	QCoreApplication::setApplicationName("quickhttpd");

	std::vector<std::string> shv_args = NecroLog::setCLIOptions(argc, argv);
	QStringList args;
	for(const auto &s : shv_args)
		args << QString::fromStdString(s);

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
		qfInfo() << "Undefined argument:" << s;
	}

	// Uncaught exception is intentional here
	if(!cli_opts.loadConfigFile())
		return 1;

	Application app(argc, argv, &cli_opts);

	return app.exec();
}
