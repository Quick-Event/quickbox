#include "stageresultshtmlexporter.h"

#include "../og/timems.h"
#include "../resultstatus.h"

#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/log.h>

#include <QDir>

namespace quickevent {
namespace core {
namespace exporters {

StageResultsHtmlExporter::StageResultsHtmlExporter(QObject *parent)
	: Super(parent)
{
	setReportTitle(tr("Results"));
	setReportDir("results");
}

void StageResultsHtmlExporter::exportClass(int class_id, const QVariantList &class_links)
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
	qs.replace("{{stage_id}}", QString::number(currentStage()));
	qs.replace("{{class_id}}", QString::number(class_id));
	qf::core::sql::Query q = execSql(qs);
	if(q.next()) {
		QString class_name = q.value("classes.name").toString();
		html_body.insert(html_body.length(), QVariantList{"h1", tr("E%1 %2 %3").arg(currentStage()).arg(class_name).arg(reportTitle())});
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
		qs2.replace("{{stage_id}}", QString::number(currentStage()));
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
			quickevent::core::ResultStatus result_status(q2);
			QString status = result_status.toHtmlExportString();
			int time_ms = q2.value(QStringLiteral("timeMs")).toInt();
			QString stime = og::TimeMs(time_ms).toString('.');
			QString spos;
			if(result_status.isOk()) {
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
		QString sub_dir = QString("E%1/results").arg(currentStage());
		QDir html_dir(outDir());
		html_dir.mkpath(sub_dir);
		QFile f(html_dir.absolutePath() + '/' + sub_dir + '/' + normalizeClassName(class_name) + ".html");
		qfInfo() << "Generating:" << f.fileName();
		if(f.open(QFile::WriteOnly)) {
			f.write(html.toUtf8());
		}
		else {
			qfError() << "Cannot open file" << f.fileName() + "for writing.";
		}
	}
}

}}}
