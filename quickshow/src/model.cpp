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
#include <QDebug>

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
	QString qs = "SELECT id FROM classes ORDER BY name";
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
		qCritical() << "Categories load ERROR";
		return false;
	}
	Application *app = Application::instance();
	int cat_id_to_load = m_categoriesToProceed.takeFirst().toInt();
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
			qCritical() << "Entry for classname" << cat_id_to_load << "does not exist !!!";
		}
	}
	{
		QString qs;
		qf::core::sql::QueryBuilder qb;
		if(app->cliOptions()->profile() == QLatin1String("results")) {
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
	first_run = false;
	return true;
}

