#include "model.h"
#include "application.h"
#include "appclioptions.h"

#include <qf/core/utils.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/log.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringBuilder>

Model::Model(QObject *parent) :
	QObject(parent)
{
	m_shiftOffset = -1;
}

void Model::shift()
{
	m_shiftOffset++;
	//qDebug() << "shift offset:" << f_shiftOffset;
}

QVariantMap Model::data(int index)
{
	QVariantMap ret;
	if(m_shiftOffset < 0)
		return ret;
	while((m_shiftOffset + index) >= m_storage.count()) {
		if(!addCategoryToStorage()) break;
	}
	int ix = m_shiftOffset + index;
	if(ix < m_storage.count()) {
		ret = m_storage[ix].toMap();
	}
	return ret;
}

void Model::reloadCategories()
{
	m_categoriesToProceed.clear();
	Application *app = Application::instance();
	AppCliOptions *cli = app->cliOptions();
	QString where;
	if(cli->classesLike_isset())
		where += "name LIKE '" + cli->classesLike() + "'";
	if(cli->classesNotLike_isset()) {
		if(!where.isEmpty())
			where += " AND ";
		where += "name NOT LIKE '" + cli->classesNotLike() + "'";
	}
	if(cli->classesIn_isset()) {
		if(!where.isEmpty())
			where += " AND ";
		where += "name IN " + cli->classesIn();
	}
	QString qs = "SELECT id FROM classes";
	if(!where.isEmpty())
		qs += " WHERE " + where;
	qs += " ORDER BY name";
	qfInfo() << "loading clases:" << qs;
	QSqlQuery q = app->execSql(qs);
	while(q.next()) {
		m_categoriesToProceed << q.value(0).toString();
	}
}

bool Model::addCategoryToStorage()
{
	static bool first_run = true;
	m_storage = m_storage.mid(m_shiftOffset);
	m_shiftOffset = 0;

	if(m_categoriesToProceed.isEmpty())
		reloadCategories();
	if(m_categoriesToProceed.isEmpty()) {
		qfError() << "Categories load ERROR";
		return false;
	}
	Application *app = Application::instance();
	bool is_relay = app->eventInfo().value("isRelay").toBool();
	int cat_id_to_load = m_categoriesToProceed.takeFirst().toInt();
	if (is_relay) {
		qf::core::sql::QueryBuilder qb;
		QString qs = "select cd.relaylegcount as leg_count from classdefs as cd where cd.classid = {{class_id}} and cd.stageid = {{stage_id}}";
		qfInfo() << "Get leg count for category:" << qs;
		qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
		qs.replace("{{class_id}}", QString::number(cat_id_to_load));
		qf::core::sql::Query q = app->execSql(qs);
		q.next();
		int leg_count_from_class = q.value("leg_count").toInt();
		for (int leg_num = 1; leg_num <= leg_count_from_class; leg_num++) {
	QVariantMap category_map;
	{
		qf::core::sql::QueryBuilder qb;
				QString qs = "select c.name || '-' || '{{leg_num}}' as name, 0 as length, 0 as climb from classes as c where c.id = {{class_id}}";
				qfInfo() << "Get category:" << qs;
				qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
				qs.replace("{{class_id}}", QString::number(cat_id_to_load));
				qs.replace("{{leg_num}}", QString::number(leg_num));
		if(first_run)
			qfInfo() << "classes:" << qs;
		qf::core::sql::Query q = app->execSql(qs);
		if(q.next()) {
			QVariantMap m;
			category_map = q.values();
			m["type"] = "classInfo";
			m["record"] = category_map;
			m_storage << m;
		}
		else {
			qfError() << "Entry for classname" << cat_id_to_load << "does not exist !!!";
		}
	}
	{
		QString qs;
		qf::core::sql::QueryBuilder qb;
		if(app->cliOptions()->profile() == QLatin1String("results")) {
					qs = "SELECT c.registration, c.lastName, c.firstName, r.*, (finishTimeMs - starttimemin*60*1000) as legFinishTimeMs, "
						 "relay.disqualified as relaydisqualified, "
						 "rel.club || ' ' || rel.name as relayname "
						 "FROM competitors as c join runs as r on r.competitorId = c.id "
						 "join relays as rel on r.relayid = rel.id "
						 "join classdefs as cd on cd.classid = rel.classId "
						 "join (select relayid, bool_or(disqualified) as disqualified from runs r group by relayid) relay on r.relayid = relay.relayid "
						 "where r.isRunning and r.finishTimeMs>0 and rel.classId = {{class_id}} and r.leg = {{leg_num}} "
						 "order by r.notCompeting, relaydisqualified, r.finishtimeMs";
					if(first_run)
						qfInfo() << "results:" << qs;
				}
				else {
			qb.select2("competitors", "registration, lastName, firstName")
					//.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
					.select2("runs", "*")
					.from("competitors")
							.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning", "JOIN")
					.where("competitors.classId={{class_id}}")
							.orderBy("runs.startTimeMs");
			qs = qb.toString();
			if(first_run)
				qfInfo() << "startlist:" << qs;
		}
		qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
		qs.replace("{{class_id}}", QString::number(cat_id_to_load));
				qs.replace("{{leg_num}}", QString::number(leg_num));
		qf::core::sql::Query q = app->execSql(qs);
		int pos = 0;
		while(q.next()) {
			QVariantMap m;
			//QVariantMap detail_map = app->sqlRecordToMap(rec);
			QVariantMap detail_map = q.values();
			detail_map["pos"] = ++pos;
			m["type"] = app->cliOptions()->profile();
			m["record"] = detail_map;
			/// pridej k detailu i kategorii, protoze na prvnim miste listu se zobrazuje vzdy zahlavi aktualni kategorie kvuli prehlednosti
			//m["category"] = category_map;
			m_storage << m;
		}
	}
		}
	} else { //event type 'individual'
		QVariantMap category_map;
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("classes", "name")
					//.select2("classdefs", "")
					.select2("courses", "length, climb")
					.from("classes")
					.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
					.join("classdefs.courseId", "courses.id")
					.where("classes.id={{class_id}}");
			QString qs = qb.toString();
			qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
			qs.replace("{{class_id}}", QString::number(cat_id_to_load));
			if(first_run)
				qfInfo() << "classes:" << qs;
			qf::core::sql::Query q = app->execSql(qs);
			if(q.next()) {
				QVariantMap m;
				category_map = q.values();
				m["type"] = "classInfo";
				m["record"] = category_map;
				m_storage << m;
			}
			else {
				qfError() << "Entry for classname" << cat_id_to_load << "does not exist !!!";
			}
		}
		{
			QString qs;
			qf::core::sql::QueryBuilder qb;
			if(app->cliOptions()->profile() == QLatin1String("results")) {
				qfInfo() << "vytazeni zavodniku pro kategorii" << qs;
				qb.select2("competitors", "registration, lastName, firstName")
						//.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
						.select2("runs", "*")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning AND runs.finishTimeMs>0", "JOIN")
						.where("competitors.classId={{class_id}}")
						.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
				qs = qb.toString();
				if(first_run)
					qfInfo() << "results:" << qs;
			}
			else {
				qb.select2("competitors", "registration, lastName, firstName")
						//.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
						.select2("runs", "*")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning", "JOIN")
						.where("competitors.classId={{class_id}}")
						.orderBy("runs.startTimeMs");
				qs = qb.toString();
				if(first_run)
					qfInfo() << "startlist:" << qs;
			}
			qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
			qs.replace("{{class_id}}", QString::number(cat_id_to_load));
			qf::core::sql::Query q = app->execSql(qs);
			int pos = 0;
			while(q.next()) {
				QVariantMap m;
				//QVariantMap detail_map = app->sqlRecordToMap(rec);
				QVariantMap detail_map = q.values();
				detail_map["pos"] = ++pos;
				m["type"] = app->cliOptions()->profile();
				m["record"] = detail_map;
				/// pridej k detailu i kategorii, protoze na prvnim miste listu se zobrazuje vzdy zahlavi aktualni kategorie kvuli prehlednosti
				//m["category"] = category_map;
				m_storage << m;
			}
		}
	}
	first_run = false;
	return true;
}

