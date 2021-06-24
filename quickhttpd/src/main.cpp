#include "application.h"
#include "appclioptions.h"

#include <qf/core/log.h>
#include <necrolog.h>

#include <QLocale>
#include <QSettings>
#include <QTranslator>

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

	QString lc_name;
    {
		if(cli_opts.locale_isset())
        	lc_name = cli_opts.locale();
		if(lc_name.isEmpty() || lc_name == QLatin1String("system"))
        	lc_name = QLocale::system().name();
        QString app_translations_path = QCoreApplication::applicationDirPath() + "/translations";

        qfInfo() << "Loading translations for:" << lc_name;
        QTranslator *qt_translator = new QTranslator(&app);
        QString tr_name = "libquickeventcore." + lc_name;
        bool ok = qt_translator->load(tr_name, app_translations_path);
        if(ok) {
            ok = app.installTranslator(qt_translator);
            qfInfo() << "Installing translator file:" << tr_name << " ... " << (ok ? "OK" : "ERROR");
        }
        else {
            qfInfo() << "Error loading translator file: " << (app_translations_path + '/' + tr_name);
        }
    }

	return app.exec();
}
