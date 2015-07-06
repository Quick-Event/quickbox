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

	{
		QString config_dir = cli_opts.configDir();
		if(config_dir.isEmpty())
			config_dir = cli_opts.applicationDir();
		QString config_file = cli_opts.config();
		qfInfo() << "config:" << config_file << "config-dir:" << config_dir;
		if(!config_file.isEmpty()) {
			if(!config_file.contains('.'))
				config_file += ".conf";
			config_file = config_dir + '/' + config_file;
			QFile f(config_file);
			qfInfo() << "Checking presence of config file:" << f.fileName();
			if(f.open(QFile::ReadOnly)) {
				qfInfo() << "Reading config file:" << f.fileName();
				QString str = QString::fromUtf8(f.readAll());
				str = qf::core::Utils::removeJsonComments(str);
				qfDebug() << str;
				QJsonParseError err;
				auto jsd = QJsonDocument::fromJson(str.toUtf8(), &err);
				if(err.error == QJsonParseError::NoError) {
					cli_opts.mergeConfig(jsd.toVariant().toMap());
				}
				else {
					qfError() << "Error parsing config file:" << f.fileName() << "on offset:" << err.offset << err.errorString();
					return 1;
				}
			}
		}
	}

	Application app(argc, argv, &cli_opts);

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("ctx_app", &app);
	engine.load(QUrl(QStringLiteral("quickshow-data/qml/main.qml")));

	return app.exec();
}
