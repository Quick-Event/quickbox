#include "stageresultscsvexporter.h"

#include "../og/timems.h"
#include "../runstatus.h"

#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/log.h>

#include <QDir>
#include <QTextStream>

namespace quickevent {
namespace core {
namespace exporters {

StageResultsCsvExporter::StageResultsCsvExporter(QObject *parent)
	: Super(parent)
{
}

void StageResultsCsvExporter::generateCsvMulti()
{
	prepareExport();
	exportClasses(false);
}

void StageResultsCsvExporter::generateCsvSingle()
{
	prepareExport();
	exportClasses(true);
}

void StageResultsCsvExporter::exportClasses(bool single_file)
{
	QString where;
	if(!classesLike().isEmpty())
		where += "name LIKE '" + classesLike() + "'";
	if(!classesNotLike().isEmpty()) {
		if(!where.isEmpty())
			where += " AND ";
		where += "name NOT LIKE '" + classesNotLike() + "'";
	}
	QString qs = "SELECT id, name FROM classes";
	if(!where.isEmpty())
		qs += " WHERE " + where;
	qs += " ORDER BY name";
	qfDebug() << "loading clases:" << qs;

	QString sub_dir = QString("E%1").arg(currentStage());
	QDir csv_dir(outDir());
	csv_dir.mkpath(sub_dir);

	if (single_file) {
		QFile f_csv(csv_dir.absolutePath() + '/' + sub_dir + '/' + "results.csv");
		qfInfo() << "Generating:" << f_csv.fileName();
		if (!f_csv.open(QFile::WriteOnly))
			qfError() << "Cannot open file" << f_csv.fileName() + "for writing.";
		QTextStream csv(&f_csv);
		csv.setCodec("UTF-8");
		exportCsvHeader(csv);
		QSqlQuery q = execSql(qs);
		while(q.next()) {
			int class_id = q.value("id").toInt();
			exportClass(class_id, csv);
		}
	}
	else { // multiple files
		QSqlQuery q = execSql(qs);
		while(q.next()) {
			int class_id = q.value("id").toInt();
			QString class_name = q.value("name").toString();
			QFile f_csv(csv_dir.absolutePath() + '/' + sub_dir + '/' + normalizeClassName(class_name) + ".csv");
			qfInfo() << "Generating:" << f_csv.fileName();
			if (!f_csv.open(QFile::WriteOnly))
				qfError() << "Cannot open file" << f_csv.fileName() + "for writing.";
			QTextStream csv(&f_csv);
			csv.setCodec("UTF-8");
			exportCsvHeader(csv);
			exportClass(class_id, csv);
		}
	}
}

void StageResultsCsvExporter::exportClass(int class_id, QTextStream &csv)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("classes", "name")
			.select2("courses", "length, climb")
			.from("classes")
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
			.join("classdefs.courseId", "courses.id")
			.where("classes.id={{class_id}}");
	QString qs = qb.toString();
	qs.replace("{{stage_id}}", QString::number(currentStage()));
	qs.replace("{{class_id}}", QString::number(class_id));
	qf::core::sql::Query q = execSql(qs);
	if(q.next()) {
		QString class_name = q.value("classes.name").toString();
		qf::core::sql::QueryBuilder qb2;
		qb2.select2("competitors", "registration, lastName, firstName, country, club")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.select2("runs", "*")
				.select2("clubs","name, abbr")
				.from("competitors")
				.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning AND runs.finishTimeMs>0", "JOIN")
				.where("competitors.classId={{class_id}}")
				.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
		QString qs2 = qb2.toString();
		qs2.replace("{{stage_id}}", QString::number(currentStage()));
		qs2.replace("{{class_id}}", QString::number(class_id));

		qf::core::sql::Query q2 = execSql(qs2);
		int pos = 0;
		int prev_time_ms = 0;
		QString spos; // keep last number when same time
		while(q2.next()) {
			pos++;
			auto run_status = quickevent::core::RunStatus::fromQuery(q2);
			int time_ms = q2.value(QStringLiteral("timeMs")).toInt();
			QString stime = og::TimeMs(time_ms).toString('.');
			if(run_status.isOk()) {
				if(time_ms != prev_time_ms)
					spos = QString::number(pos);
			}
			else {
				spos = "-";
			}
			prev_time_ms = time_ms;

			QString club = q2.value("competitors.club").toString();
			if (club.isEmpty()) {
				club = q2.value("clubs.name").toString();
				if (club.isEmpty())
					club = q2.value("competitors.registration").toString().left(3);
			}
			csv << class_name << m_separator;
			csv << spos << m_separator;
			csv << q2.value("competitorName").toString() << m_separator;
			csv << club << m_separator;
			csv << q2.value("competitors.country").toString() << m_separator;
			csv << stime << m_separator;
			csv << run_status.toHtmlExportString();
			csv << Qt::endl;
		}
	}
}

void StageResultsCsvExporter::exportCsvHeader(QTextStream &csv)
{
	csv << "Class" << m_separator;
	csv << "Position" << m_separator;
	csv << "Name" << m_separator;
	csv << "Club" << m_separator;
	csv << "Country" << m_separator;
	csv << "Time" << m_separator;
	csv << "Status";
	csv << Qt::endl;
}

}}}
