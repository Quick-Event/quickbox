#include "model.h"
#include "application.h"
#include "appclioptions.h"

#include <qf/core/utils.h>
#include <qf/core/sql/querybuilder.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringBuilder>
#include <QDebug>

Model::Model(QObject *parent) :
	QObject(parent)
{
	f_shiftOffset = -1;
}

void Model::shift()
{
	f_shiftOffset++;
	//qDebug() << "shift offset:" << f_shiftOffset;
}

QVariant Model::data(int index)
{
	QVariant ret;
	while((f_shiftOffset + index) >= f_storage.count()) {
		if(!addCategoryToStorage()) break;
	}
	int ix = f_shiftOffset + index;
	if(ix < f_storage.count()) {
		ret = f_storage[ix];
	}
	return ret;
}

void Model::reloadCategories()
{
	f_categoriesToProceed.clear();
	Application *app = Application::instance();
	QString qs = "SELECT id FROM classes ORDER BY name";
	QSqlQuery q = app->execSql(qs);
	while(q.next()) {
		f_categoriesToProceed << q.value(0).toString();
	}
}

bool Model::addCategoryToStorage()
{
	static bool first_run = true;
	f_storage = f_storage.mid(f_shiftOffset);
	f_shiftOffset = 0;

	if(f_categoriesToProceed.isEmpty()) {reloadCategories();}
	if(f_categoriesToProceed.isEmpty()) {
		qCritical() << "Categories load ERROR";
		return false;
	}
	Application *app = Application::instance();
	int cat_id_to_load = f_categoriesToProceed.takeFirst().toInt();
	QVariantMap category_map;
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("classes", "*")
				.select2("classdefs", "*")
				.select2("courses", "*")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
				.join("classdefs.courseId", "courses.id")
				.where("classes.id={{class_id}}");
		QString qs = qb.toString();
		if(first_run)
			qfInfo() << "classes:" << qs;
		qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
		qs.replace("{{class_id}}", QString::number(cat_id_to_load));
		QSqlQuery q = app->execSql(qs);
		if(q.next()) {
			QSqlRecord rec = q.record();
			QVariantMap m;
			category_map = app->sqlRecordToMap(rec);
			m["type"] = "category";
			m["category"] = category_map;
			f_storage << m;
		}
		else {
			qCritical() << "Entry for classname" << cat_id_to_load << "does not exist !!!";
		}
	}
	{
		QString qs;
		qf::core::sql::QueryBuilder qb;
		if(app->cliOptions()->profile() == "results") {
			qb.select2("competitors", "registration, lastName, firstName")
					.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
					.select2("runs", "*")
					.from("competitors")
					//.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
					.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
					.where("competitors.classId={{class_id}}")
					.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
			qs = qb.toString();
			if(first_run)
				qfInfo() << "results:" << qs;
		}
		else {
			qb.select2("competitors", "registration, lastName, firstName")
					.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
					.select2("runs", "*")
					//.select2("clubs", "name")
					.from("competitors")
					//.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
					.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace", "JOIN")
					//.join("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
					.where("competitors.classId={{class_id}}")
					.orderBy("runs.startTimeMs");
			qs = qb.toString();
			if(first_run)
				qfInfo() << "startlist:" << qs;
		}
		qs.replace("{{stage_id}}", QString::number(app->cliOptions()->stage()));
		qs.replace("{{class_id}}", QString::number(cat_id_to_load));
		QSqlQuery q = app->execSql(qs);
		int pos = 0;
		while(q.next()) {
			QSqlRecord rec = q.record();
			QVariantMap m;
			QVariantMap detail_map = app->sqlRecordToMap(rec);
			detail_map["pos"] = ++pos;
			m["type"] = "detail";
			m["detail"] = detail_map;
			/// pridej k detailu i kategorii, protoze na prvnim miste listu se zobrazuje vzdy zahlavi aktualni kategorie kvuli prehlednosti
			m["category"] = category_map;
			f_storage << m;
		}
	}
	first_run = false;
	return true;
}

