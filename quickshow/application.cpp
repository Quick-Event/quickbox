#include "application.h"
#include "appclioptions.h"
#include "model.h"

#include <qf/core/log.h>
#include <qf/core/sql/connection.h>

#include <QSettings>
#include <QStringList>
#include <QStringBuilder>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QDebug>

Application::Application(int &argc, char **argv, AppCliOptions *cli_opts)
	: Super(argc, argv)
	, m_cliOptions(cli_opts)
{
	m_model = new Model(this);
	//setProperty("model", QVariant::fromValue(qobject_cast<QObject*>(m)));
}
/*
void Application::initSettings()
{
	QSettings ss;
	if(ss.childGroups().isEmpty()) {
		ss.beginGroup("application");
		ss.setValue("profile", "results");
		ss.endGroup();
		ss.beginGroup("profile");
		ss.beginGroup("results");
		ss.beginGroup("query");
		ss.setValue("categories", "SELECT classname FROM classes ORDER BY classname");
		ss.setValue("category", "SELECT classdefs.* FROM classdefs"
								" WHERE classdefs.etap={{event/etap}}"
								" AND classdefs.classname='{{CATEGORY}}'");
		ss.setValue("details", "SELECT runners.*, laps.* FROM runners JOIN laps ON runners.id = laps.idrunner"
							   " WHERE laps.etap={{event/etap}}"
							   " AND runners.classname='{{CATEGORY}}'"
							   "  AND laps.IDSI > 0"
							   "  AND laps.status != 'NOT_RUN'"
							   " ORDER BY runners.flag, laps.status, laps.laptime, laps.laptimems");

		ss.endGroup();
		ss.endGroup();

		ss.beginGroup("startlist");
		ss.beginGroup("query");
		ss.setValue("categories", "SELECT classname FROM classes ORDER BY classname");
		ss.setValue("category", "SELECT classdefs.* FROM classdefs"
								" WHERE classdefs.etap={{event/etap}}"
								" AND classdefs.classname='{{CATEGORY}}'");
		ss.setValue("details", "SELECT runners.id, runners.name, runners.course, runners.reg, runners.flag"
							   " ,laps.start, laps.laptime, laps.status, laps.idsi"
							   " FROM runners JOIN laps ON runners.id=laps.idrunner"
							   " WHERE laps.etap={{event/etap}}"
							   "  AND classname='{{CATEGORY}}'"
							   " ORDER BY laps.start");
		ss.endGroup();
		ss.endGroup();

		ss.endGroup();
	}
}
*/
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
		if(cliOptions()->eventName().isEmpty())
			qfFatal("Event name is empty!");
		db = QSqlDatabase::addDatabase("QPSQL");
		db.setHostName(cliOptions()->host());
		db.setPort(cliOptions()->port());
		db.setDatabaseName(cliOptions()->database());
		db.setUserName(cliOptions()->user());
		db.setPassword(cliOptions()->password());
		//QString opts = "QF_CODEC_NAME=cp1250;QF_MYSQL_SET_NAMES=latin1";
		qfInfo() << "connecting to:" << db.hostName() << db.port() << db.userName() << "...";// << db.password();
		//db.setConnectOptions(opts);
		//db.setPassword("toor");
		bool ok = db.open();
		if(!ok) {
			qfError() << "ERROR open database:" << db.lastError().text();
		}
		else {
			qfInfo() << "\tSetting current schema to" << cliOptions()->eventName();
			db.setCurrentSchema(cliOptions()->eventName());
			if(db.currentSchema() != cliOptions()->eventName()) {
				qfError() << "ERROR open event:" << cliOptions()->eventName();
			}
			else {
				qfInfo() << "\tOK";
				setSqlConnected(true);
			}
		}
	}
	return db;
}

QSqlQuery Application::execSql(const QString &query_str)
{
	QString qs = query_str;
	{
		static QRegExp rx_id_placeholders("\\{\\{([A-Za-z0-9\\.\\_\\/]+)\\}\\}");
		int pos = 0;
		while((pos = rx_id_placeholders.indexIn(query_str, pos)) != -1) {
			QString fld_name = rx_id_placeholders.cap(1);
			qs.replace(rx_id_placeholders.cap(0), cliOptions()->value(fld_name).toString());
			pos += rx_id_placeholders.matchedLength();
		}
	}
	QSqlQuery q(sqlConnetion());
	if(!q.exec(qs)) {
		QSqlError err = q.lastError();
		qfError() << "SQL ERROR:" << err.text();
		//qCritical() << ("QUERY: "%q.lastQuery());
	}
	return q;
}

QVariantMap Application::sqlRecordToMap(const QSqlRecord &rec)
{
	QVariantMap ret;
	for(int i=0; i<rec.count(); i++) {
		QString fld_name = rec.fieldName(i).section('.', -1, -1).toLower();
		QVariant v = rec.value(i);
		//if(v.type() == QVariant::String) v = recodeSqlString(v.toString());
		ret[fld_name] = v;
		//qDebug() << fld_name << "->" << v.toString();
	}
	return ret;
}

QVariant Application::eventInfo()
{
	static QVariantMap info;
	if(info.isEmpty()) {
		QSqlQuery q = execSql("SELECT ckey, cvalue FROM config WHERE ckey LIKE 'event.%'");
		while(q.next())
			info[q.value(0).toString().mid(6)] = q.value(1);
		info["profile"] = profile();
	}
	return info;
}

QString Application::profile()
{
	return cliOptions()->profile();
}

QVariant Application::cliOptionValue(const QString &option_name)
{
	QVariant ret = cliOptions()->value(option_name);
	return ret;
}

QObject *Application::model()
{
	return m_model;
}
