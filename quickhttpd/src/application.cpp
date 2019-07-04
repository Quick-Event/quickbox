#include "application.h"
#include "appclioptions.h"
#include "httpserver.h"

#include <quickevent/core/exporters/stagestartlisthtmlexporter.h>
#include <quickevent/core/exporters/stageresultshtmlexporter.h>

#include <qf/core/log.h>
#include <qf/core/sql/connection.h>

#include <QSqlError>
#include <QTimer>

Application::Application(int &argc, char **argv, AppCliOptions *cli_opts)
	: Super(argc, argv)
	, m_cliOptions(cli_opts)
{
	int refresh_time_msec = cli_opts->refreshTime();
	qfInfo() << "HTML dir refresh time:" << refresh_time_msec << "msec";
	if(refresh_time_msec >= 1000) {
		QTimer *rft = new QTimer(this);
		connect(rft, &QTimer::timeout, this, &Application::generateHtml);
		rft->start(refresh_time_msec);
		generateHtml();
	}
	else {
		generateHtml();
		quit();
		return;
	}
	if(cli_opts->httpPort() > 0) {
		HttpServer *srv = new HttpServer(this);
		qfInfo() << "HTTP server is listenning on port:" << cli_opts->httpPort();
		srv->listen(QHostAddress::Any, cli_opts->httpPort());
	}
}

Application *Application::instance()
{
	Application *ret = qobject_cast<Application*>(Super::instance());
	if(!ret)
		qFatal("Invalid Application instance");
	return ret;
}

qf::core::sql::Connection Application::sqlConnetion()
{
	qf::core::sql::Connection db = qf::core::sql::Connection::forName();
	if(!db.isValid()) {
		db = QSqlDatabase::addDatabase(cliOptions()->sqlDriver());
		db.setHostName(cliOptions()->sqlHost());
		db.setPort(cliOptions()->sqlPort());
		db.setDatabaseName(cliOptions()->sqlDatabase());
		db.setUserName(cliOptions()->sqlUser());
		db.setPassword(cliOptions()->sqlPassword());
		qfInfo() << "connecting to database:"
				 << db.databaseName()
				 << "at:" << (db.userName() + '@' + db.hostName() + ':' + QString::number(db.port()))
				 << "driver:" << db.driverName()
				 << "...";// << db.password();
		bool ok = db.open();
		if(!ok) {
			qfError() << "ERROR open database:" << db.lastError().text();
		}
		else {
			if(!cliOptions()->sqlDriver().endsWith(QLatin1String("SQLITE"))) {

				QString event_name = cliOptions()->eventName();
				if(event_name.isEmpty()) {
					qfError("Event name is empty!");
				}
				else {
					qfInfo() << "\tSetting current schema to" << cliOptions()->eventName();
					db.setCurrentSchema(cliOptions()->eventName());
					if(db.currentSchema() != cliOptions()->eventName()) {
						qfError() << "ERROR open event:" << cliOptions()->eventName();
					}
				}
			}
			if(ok) {
				qfInfo() << "\tOK";
				setSqlConnected(true);
			}
		}
	}
	return db;
}

void Application::generateHtml()
{
	{
		quickevent::core::exporters::StageStartListHtmlExporter exp;
		exp.setSqlConnection(sqlConnetion());
		exp.setOutDir(cliOptions()->htmlDir());
		exp.setClassesLike(cliOptions()->classesLike());
		exp.setClassesNotLike(cliOptions()->classesNotLike());
		exp.generateHtml();
	}
	{
		quickevent::core::exporters::StageResultsHtmlExporter exp;
		exp.setSqlConnection(sqlConnetion());
		exp.setOutDir(cliOptions()->htmlDir());
		exp.setClassesLike(cliOptions()->classesLike());
		exp.setClassesNotLike(cliOptions()->classesNotLike());
		exp.generateHtml();
	}
}



