#include "application.h"
#include "appclioptions.h"
#include "httpserver.h"

#include <qf/core/log.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/collator.h>

#include <QSettings>
#include <QStringList>
#include <QStringBuilder>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QDebug>
#include <QTimer>
#include <QDir>

static QString timeMsToString(int time_ms, QChar sec_sep = ':', QChar msec_sep = QChar())
{
	if(time_ms == 0)
		return QString();

	int msec = time_ms % 1000;
	int sec = (time_ms / 1000) % 60;
	int min = time_ms / (1000 * 60);
	QString ret = QString::number(min) + sec_sep;
	if(sec < 10)
		ret += '0';
	ret += QString::number(sec);
	if(!msec_sep.isNull()) {
		ret += msec_sep;
		if(msec < 100)
			ret += '0';
		if(msec < 10)
			ret += '0';
		ret += QString::number(msec);
	}
	return ret;
}

Application::Application(int &argc, char **argv, AppCliOptions *cli_opts)
	: Super(argc, argv)
	, m_cliOptions(cli_opts)
{
	int refresh_time_msec = cli_opts->refreshTime();
	refresh_time_msec = 60 * 1000;
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

qf::core::sql::Query Application::execSql(const QString &query_str)
{
	QString qs = query_str;
	qf::core::sql::Query q(sqlConnetion());
	if(!q.exec(qs)) {
		QSqlError err = q.lastError();
		qfError() << "SQL ERROR:" << err.text();
		//qCritical() << ("QUERY: "%q.lastQuery());
		::exit(-1);
	}
	return q;
}

QVariantMap Application::eventInfo()
{
	static QVariantMap info;
	if(info.isEmpty()) {
		QSqlQuery q = execSql("SELECT ckey, cvalue FROM config WHERE ckey LIKE 'event.%'");
		while(q.next())
			info[q.value(0).toString().mid(6)] = q.value(1);
	}
	return info;
}
/*
static QVariantList generate_html_table(const QString &title, const QStringList &flds, const QVariantList &rows)
{
	QVariantList div{QStringLiteral("div")};
	div.insert(div.length(), QVariantList{QStringLiteral("h2"), title});
	QVariantList table{QStringLiteral("table")};
	QVariantList header{QStringLiteral("tr")};
	for(auto fld : flds)
		header.insert(header.length(), QVariantList{QStringLiteral("th"), fld});
	table.insert(table.length(), header);
	table << rows;
	div.insert(div.length(), table);
	return div;
}
*/
void Application::generateHtml()
{
	QDir html_dir(cliOptions()->htmlDir());
	if(!html_dir.exists()) {
		qfInfo() << "creating HTML dir:" << cliOptions()->htmlDir();
		if(!QDir().mkpath(cliOptions()->htmlDir())) {
			qfError() << "Cannot create HTML dir:" << cliOptions()->htmlDir();
			return;
		}
		html_dir = QDir(cliOptions()->htmlDir());
		if(!html_dir.exists()) {
			qfError() << "Author event doesn't know, how to use QDir API.";
			return;
		}
	}
	QVariantMap event_info = eventInfo();
	qfDebug() << event_info;
	//QString event_name = event_info.value("name").toString();
	//ui->lblHeadRight->setText(event_info.value("date").toString());
	//int stage_cnt = event_info.value("stageCount").toInt();
	int curr_stage = cliOptions()->stage();
	if(!cliOptions()->stage_isset()) {
		curr_stage = event_info.value("currentStageId").toInt();
		if(curr_stage == 0)
			curr_stage = 1;
		qfInfo() << "Setting stage to:" << curr_stage;
		cliOptions()->setStage(curr_stage);
	}

	QVariantList html_body = QVariantList() << QStringLiteral("body");
	html_body.insert(html_body.length(), QVariantList() << QStringLiteral("body"));

	exportClassesResults(html_dir, curr_stage);
}

static QString name7(const QString name)
{
	QString ret = name;
	ret.replace(' ', '-');
	return QString::fromUtf8(qf::core::Collator::toAscii7(QLocale::Czech, ret, true));
}

void Application::exportClassesResults(const QDir &html_dir, int stage_no)
{
	QString where;
	if(cliOptions()->classesLike_isset())
		where += "name LIKE '" + cliOptions()->classesLike() + "'";
	if(cliOptions()->classesNotLike_isset()) {
		if(!where.isEmpty())
			where += " AND ";
		where += "name NOT LIKE '" + cliOptions()->classesNotLike() + "'";
	}
	QString qs = "SELECT id, name FROM classes";
	if(!where.isEmpty())
		qs += " WHERE " + where;
	qs += " ORDER BY name";
	qfDebug() << "loading clases:" << qs;

	QVariantList class_links;
	QList<int> class_ids;
	QSqlQuery q = execSql(qs);
	while(q.next()) {
		int class_id = q.value("id").toInt();
		class_ids << class_id;
		QString class_name = q.value("name").toString();
		QString class_name_ascii7 = name7(class_name);

		class_links.insert(class_links.length(), QVariantList{"a", QVariantMap{{"href", class_name_ascii7 + ".html"}}, class_name});
	}
	for(int class_id : class_ids) {
		exportClassResults(html_dir, stage_no, class_id, class_links);
		exportClassStartList(html_dir, stage_no, class_id, class_links);
	}

	QVariantMap event_info = eventInfo();
	{
		QVariantList html_body = QVariantList() << QStringLiteral("body");
		html_body.insert(html_body.length(), QVariantList{"h1", tr("E%1 Results").arg(stage_no)});
		html_body.insert(html_body.length(), QVariantList{"h2", event_info.value("name")});
		html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("place")});
		html_body.insert(html_body.length(), QVariantList{"p"} << class_links);
		//qfInfo() << html_body;
		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(tr("E%1 Results").arg(stage_no));
		QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
		QString sub_dir = QString("E%1/results").arg(stage_no);
		html_dir.mkpath(sub_dir);
		QFile f(html_dir.absolutePath() + '/' + sub_dir + "/index.html");
		qfInfo() << "Generating:" << f.fileName();
		if(f.open(QFile::WriteOnly)) {
			f.write(html.toUtf8());
		}
		else {
			qfError() << "Cannot open file" << f.fileName() + "for writing.";
		}
	}
	{
		QVariantList html_body = QVariantList() << QStringLiteral("body");
		html_body.insert(html_body.length(), QVariantList{"h1", tr("E%1 Start list").arg(stage_no)});
		html_body.insert(html_body.length(), QVariantList{"h2", event_info.value("name")});
		html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("place")});
		html_body.insert(html_body.length(), QVariantList{"p"} << class_links);
		//qfInfo() << html_body;
		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(tr("E%1 Results").arg(stage_no));
		QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
		QString sub_dir = QString("E%1/start").arg(stage_no);
		html_dir.mkpath(sub_dir);
		QFile f(html_dir.absolutePath() + '/' + sub_dir + "/index.html");
		qfInfo() << "Generating:" << f.fileName();
		if(f.open(QFile::WriteOnly)) {
			f.write(html.toUtf8());
		}
		else {
			qfError() << "Cannot open file" << f.fileName() + "for writing.";
		}
	}
}

void Application::exportClassResults(const QDir &html_dir, int stage_no, int class_id, const QVariantList &class_links)
{
	QVariantMap event_info = eventInfo();
	QVariantList html_body = QVariantList() << QStringLiteral("body");
	qf::core::sql::QueryBuilder qb;
	qb.select2("classes", "name")
			//.select2("classdefs", "")
			.select2("courses", "length, climb")
			.from("classes")
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
			.join("classdefs.courseId", "courses.id")
			.where("classes.id={{class_id}}");
	QString qs = qb.toString();
	qs.replace("{{stage_id}}", QString::number(stage_no));
	qs.replace("{{class_id}}", QString::number(class_id));
	qf::core::sql::Query q = execSql(qs);
	if(q.next()) {
		QString class_name = q.value("classes.name").toString();
		html_body.insert(html_body.length(), QVariantList{"h1", tr("E%1 %2 Results").arg(stage_no).arg(class_name)});
		html_body.insert(html_body.length(), QVariantList{"h2", event_info.value("name")});
		html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("place")});
		html_body.insert(html_body.length(), QVariantList{"p"} << class_links);
		html_body.insert(html_body.length(), QVariantList{"h3", QVariantList{"b", class_name}, tr(" length: "), q.value("courses.length").toString(), "m ", tr("climb: "), q.value("courses.climb").toString(), "m"});

		//html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("stageStart")});
		QVariantList table{"table"};

		qf::core::sql::QueryBuilder qb2;
		qb2.select2("competitors", "registration, lastName, firstName")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.select2("runs", "*")
				.from("competitors")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning AND runs.finishTimeMs>0", "JOIN")
				.where("competitors.classId={{class_id}}")
				.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
		QString qs2 = qb2.toString();
		qs2.replace("{{stage_id}}", QString::number(stage_no));
		qs2.replace("{{class_id}}", QString::number(class_id));

		QVariantList tr1{"tr",
					QVariantList{"th", tr("Pos")},
					QVariantList{"th", tr("Name")},
					QVariantList{"th", tr("Registration")},
					QVariantList{"th", tr("Time")},
					QVariantList{"th", tr("Status")},
				};
		table.insert(table.length(), tr1);
		qf::core::sql::Query q2 = execSql(qs2);
		int pos = 0;
		int prev_time_ms = 0;
		while(q2.next()) {
			pos++;
			bool disq = q2.value(QStringLiteral("disqualified")).toBool();
			bool nc = q2.value(QStringLiteral("notCompeting")).toBool();
			bool has_pos = !disq && !nc;
			QString status;
			if(nc)
				status = tr("NC");
			if(disq) {
				if(!status.isEmpty())
					status += ", ";
				status = tr("DISQ");
			}
			int time_ms = q2.value(QStringLiteral("timeMs")).toInt();
			QString stime = timeMsToString(time_ms);
			QString spos;
			if(has_pos) {
				if(time_ms != prev_time_ms)
					spos = QString::number(pos) + '.';
			}
			prev_time_ms = time_ms;
			QVariantList tr2{"tr"};
			if(pos % 2)
				tr2 << QVariantMap{{QStringLiteral("class"), QStringLiteral("odd")}};
			tr2 << QVariantList {
						QVariantList{"td", spos},
						QVariantList{"td", q2.value("competitorName")},
						QVariantList{"td", q2.value("competitors.registration")},
						QVariantList{"td", QVariantMap{{QStringLiteral("align"), QStringLiteral("right")}}, stime},
						QVariantList{"td", status},
					};
			table.insert(table.length(), tr2);
		}
		html_body.insert(html_body.length(), table);

		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(tr("Results %1").arg(q.value("classes.name").toString()));
		QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
		QString sub_dir = QString("E%1/results").arg(stage_no);
		html_dir.mkpath(sub_dir);
		QFile f(html_dir.absolutePath() + '/' + sub_dir + '/' + name7(class_name) + ".html");
		qfInfo() << "Generating:" << f.fileName();
		if(f.open(QFile::WriteOnly)) {
			f.write(html.toUtf8());
		}
		else {
			qfError() << "Cannot open file" << f.fileName() + "for writing.";
		}
	}
}

void Application::exportClassStartList(const QDir &html_dir, int stage_no, int class_id, const QVariantList &class_links)
{
	QVariantMap event_info = eventInfo();
	QVariantList html_body = QVariantList() << QStringLiteral("body");
	qf::core::sql::QueryBuilder qb;
	qb.select2("classes", "name")
			//.select2("classdefs", "")
			.select2("courses", "length, climb")
			.from("classes")
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
			.join("classdefs.courseId", "courses.id")
			.where("classes.id={{class_id}}");
	QString qs = qb.toString();
	qs.replace("{{stage_id}}", QString::number(stage_no));
	qs.replace("{{class_id}}", QString::number(class_id));
	qf::core::sql::Query q = execSql(qs);
	if(q.next()) {
		QString class_name = q.value("classes.name").toString();
		html_body.insert(html_body.length(), QVariantList{"h1", tr("E%1 %2 Start list").arg(stage_no).arg(class_name)});
		html_body.insert(html_body.length(), QVariantList{"h2", event_info.value("name")});
		html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("place")});
		html_body.insert(html_body.length(), QVariantList{"p"} << class_links);
		html_body.insert(html_body.length(), QVariantList{"h3", QVariantList{"b", class_name}, tr(" length: "), q.value("courses.length").toString(), "m ", tr("climb: "), q.value("courses.climb").toString(), "m"});

		//html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("stageStart")});
		QVariantList table{"table"};

		qf::core::sql::QueryBuilder qb2;
		qb2.select2("competitors", "registration, lastName, firstName")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.select2("runs", "*")
				.from("competitors")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning", "INNER JOIN")
				.where("competitors.classId={{class_id}}")
				.orderBy("runs.startTimeMs");
		QString qs2 = qb2.toString();
		qs2.replace("{{stage_id}}", QString::number(stage_no));
		qs2.replace("{{class_id}}", QString::number(class_id));

		QVariantList tr1{"tr",
					QVariantList{"th", tr("Start")},
					QVariantList{"th", tr("Name")},
					QVariantList{"th", tr("Registration")},
					QVariantList{"th", tr("SI")},
				};
		table.insert(table.length(), tr1);
		qf::core::sql::Query q2 = execSql(qs2);
		int pos = 0;
		while(q2.next()) {
			pos++;
			int time_ms = q2.value(QStringLiteral("startTimeMs")).toInt();
			QString stime = timeMsToString(time_ms);
			QVariantList tr2{"tr"};
			if(pos % 2)
				tr2 << QVariantMap{{QStringLiteral("class"), QStringLiteral("odd")}};
			tr2 << QVariantList {
						QVariantList{"td", QVariantMap{{QStringLiteral("align"), QStringLiteral("right")}}, stime},
						QVariantList{"td", q2.value("competitorName")},
						QVariantList{"td", q2.value("competitors.registration")},
						QVariantList{"td", QVariantMap{{QStringLiteral("align"), QStringLiteral("right")}}, q2.value("runs.siid").toString()},
					};
			table.insert(table.length(), tr2);
		}
		html_body.insert(html_body.length(), table);

		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(tr("Start list %1").arg(q.value("classes.name").toString()));
		QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
		QString sub_dir = QString("E%1/start").arg(stage_no);
		html_dir.mkpath(sub_dir);
		QFile f(html_dir.absolutePath() + '/' + sub_dir + '/' + name7(class_name) + ".html");
		qfInfo() << "Generating:" << f.fileName();
		if(f.open(QFile::WriteOnly)) {
			f.write(html.toUtf8());
		}
		else {
			qfError() << "Cannot open file" << f.fileName() + "for writing.";
		}
	}
}


