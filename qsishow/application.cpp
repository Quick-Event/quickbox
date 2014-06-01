#include "application.h"
#include "model.h"

#include <QSettings>
#include <QStringList>
#include <QStringBuilder>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QTextCodec>
#include <QDebug>

Application::Application(int &argc, char **argv)
	: QApplication(argc, argv)
{
	initSettings();
	Model *m = new Model(this);
	//m->setObjectName("model1");
	setProperty("model", QVariant::fromValue(qobject_cast<QObject*>(m)));
	//setProperty("test", "ahoj");
}

QVariant Application::getCmdLineArgValue(const QString &_long_key, const QString &_short_key)
{
	QVariant ret;
	QString short_key;
	if(!_short_key.isEmpty()) short_key = '-' + _short_key;
	QString long_key;
	if(!_long_key.isEmpty()) long_key = "--" + _long_key;
	QStringList args = arguments();
	for(int i=0; i<args.size(); i++) {
		QString arg = args[i];
		if(!long_key.isEmpty() && arg.startsWith(long_key)) {
			if(arg.length() == long_key.length()) {
				ret = true;
				break;
			}
			else if(arg[long_key.length()] == '=') {
				ret = arg.mid(long_key.length() + 1);
				break;
			}
		}
		if(!short_key.isEmpty() && arg.startsWith(short_key)) {
			if(arg.length() == short_key.length()) {
				ret = true;
				QString arg2 = args.value(i+1);
				if(!arg2.isEmpty() && arg2[0]!='-') ret = arg2;
				break;
			}
			else {
				ret = arg.mid(short_key.length());
				break;
			}
		}
	}
	qDebug() << "Getting cmd line argument" << long_key << short_key << "->" << ret.toString();
	return ret;
}

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
		/*
		SELECT * FROM (
		SELECT laps1.laptime + laps2.laptime + laps3.laptime AS slaptime, laps1.laptime AS laptime1, laps2.laptime AS laptime2, laps3.laptime AS laptime3, runners.* FROM runners
		JOIN ( SELECT * FROM laps WHERE laps.etap=1 AND laps.status = 'OK') AS laps1 ON runners.id=laps1.idrunner
		JOIN ( SELECT * FROM laps WHERE laps.etap=2 AND laps.status = 'OK') AS laps2 ON runners.id=laps2.idrunner
		JOIN ( SELECT * FROM laps WHERE laps.etap=3 AND laps.status = 'OK') AS laps3 ON runners.id=laps3.idrunner
		WHERE runners.flag='' AND  runners.classname='D21E'
		 ) AS results3
		ORDER BY slaptime;
		*/
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
	initConfigValue("application/refreshTime", "refreshTime", 1000);
	initConfigValue("application/profile", "profile", "results");
	initConfigValue("event/etap", "etap", 1);
	initConfigValue("connection/host", "host", "localhost");
	initConfigValue("connection/port", "port", 0);
	initConfigValue("connection/user", "user", "root");
	initConfigValue("connection/password", "password");
	initConfigValue("connection/database", "database");

	if(getCmdLineArgValue("save-settings", "s").toBool()) flushConfig();
}

void Application::initConfigValue(const QString &settings_path, const QString &cli_long_key, const QVariant &default_val)
{
	QVariant v = getCmdLineArgValue(cli_long_key);
	if(v.isValid()) {
		QString s;
		if(v.type() == QVariant::String) s = v.toString();
		if(!s.isEmpty()) {
			do {
				if(default_val.type()==QVariant::Int || default_val.type()==QVariant::Invalid) {
					bool ok;
					int i = s.toInt(&ok);
					if(ok) {v = i; break;}
				}
				if(default_val.type()==QVariant::Double || default_val.type()==QVariant::Invalid) {
					bool ok;
					double i = s.toDouble(&ok);
					if(ok) {v = i; break;}
				}
			} while(false);
		}
		if(v.isValid()) f_appConfig[settings_path] = v;
	}
	//else v = default_val;
}

void Application::flushConfig()
{
	QMapIterator<QString, QVariant> it(f_appConfig);
	while(it.hasNext()) {
		it.next();
		setSetting(it.key(), it.value());
	}
}

QVariant Application::appConfigValue(const QString &path, const QVariant def_val)
{
	QVariant ret = f_appConfig.value(path);
	if(!ret.isValid()) ret = setting(path, def_val);
	return ret;
}

QVariant Application::setting(const QString &path, const QVariant &default_val)
{
	QSettings ss;
	return ss.value(path, default_val);
}

void Application::setSetting(const QString &path, const QVariant &val)
{
	QSettings ss;
	ss.setValue(path, val);
}

Application* Application::instance()
{
	Application *ret = qobject_cast<Application*>(QApplication::instance());
	if(!ret) qFatal("Invalid Application instance");
	return ret;
}

QSqlDatabase Application::sqlConnetion()
{
	QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
	if(!db.isValid()) {
		Application *app = this;
		db = QSqlDatabase::addDatabase("QFMYSQL");
		db.setHostName(app->appConfigValue("connection/host").toString());
		db.setPort(app->appConfigValue("connection/port").toInt());
		db.setDatabaseName(app->appConfigValue("connection/database").toString());
		db.setUserName(app->appConfigValue("connection/user").toString());
		db.setPassword(app->appConfigValue("connection/password").toString());
		QString opts = "QF_CODEC_NAME=cp1250;QF_MYSQL_SET_NAMES=latin1";
		qDebug() << "connecting to:" << db.hostName() << db.port() << db.userName() << db.password();
		db.setConnectOptions(opts);
		//db.setPassword("toor");
		bool ok = db.open();
		if(!ok) qCritical() << "ERROR open database:" << db.lastError().text();
		else {
			//QSqlQuery q(db);
			//q.exec("SET NAMES latin1");
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
			qs.replace(rx_id_placeholders.cap(0), appConfigValue(fld_name).toString());
			pos += rx_id_placeholders.matchedLength();
		}
	}
	QSqlDatabase db = sqlConnetion();
	QSqlQuery q(db);
	if(!q.exec(qs)) {
		QSqlError err = q.lastError();
        //qCritical() << ("SQL ERROR: "%err.text());
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
#if 0
QString Application::recodeSqlString(const QString &str)
{
	QString ret = str;
	/*
	QTextCodec *tc = QTextCodec::codecForName("cp1250");
	if(!tc) qCritical() << "ERROR load codec cp1250";
	else {
		QByteArray ba = str.toLatin1();
		ba.replace('\x3a', '\x9a');
		ret = tc->toUnicode(ba);
	}
	*/
	//QByteArray ba = str.toUtf8();
	//ret.replace(QString::fromUtf8("ò"), QString::fromUtf8("ň"));
	//ret.replace("ò", "ř");
	//qDebug() << str << ret;
	return ret;
}
#endif

QVariant Application::eventInfo()
{
	static QVariantMap info;
	if(info.isEmpty()) {
		QSqlQuery q = execSql("SELECT ckey, value FROM config");
		while(q.next()) info[q.value(0).toString()] = q.value(1);
		info["profile"] = profile();
	}
	return info;
}
