#include "application.h"
#include "appclioptions.h"

#include <qf/core/utils.h>
#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QQmlContext>
#include <QSettings>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QJsonParseError>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setOrganizationName("QuickBox");
	QCoreApplication::setApplicationName("quickshow");

	QStringList args;

	QScopedPointer<qf::core::LogDevice> log_device(qf::core::FileLogDevice::install());
	args = log_device->setDomainTresholds(argc, argv);
	log_device->setPrettyDomain(true);

	AppCliOptions cli_opts;
	cli_opts.parse(args);
	if(cli_opts.isParseError()) {
		foreach(QString err, cli_opts.parseErrors())
			qfError() << err;
		return 1;
	}
	if(cli_opts.isAppBreak()) {
		return 0;
	}
	foreach(QString s, cli_opts.unusedArguments()) {
		qDebug() << "Undefined argument:" << s;
	}

	if(!cli_opts.loadConfigFile())
		return 1;

	Application app(argc, argv, &cli_opts);

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("ctx_app", &app);
	engine.load(QUrl(QStringLiteral("quickshow-data/qml/main.qml")));

	return app.exec();
}
