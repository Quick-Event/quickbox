#include "htmlfileexporter.h"

#include <qf/core/sql/query.h>
#include <qf/core/log.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/collator.h>

#include <QDir>

namespace quickevent {
namespace core {
namespace exporters {

HtmlFileExporter::HtmlFileExporter(QObject *parent)
	: QObject(parent)
{
}

QVariantMap HtmlFileExporter::eventInfo()
{
	if(m_eventInfo.isEmpty()) {
		QSqlQuery q = execSql("SELECT ckey, cvalue FROM config WHERE ckey LIKE 'event.%'");
		while(q.next())
			m_eventInfo[q.value(0).toString().mid(6)] = q.value(1);
	}
	return m_eventInfo;
}

qf::core::sql::Query HtmlFileExporter::execSql(const QString &query_str)
{
	QString qs = query_str;
	qf::core::sql::Query q(sqlConnection());
	if(!q.exec(qs)) {
		QSqlError err = q.lastError();
		qfError() << "SQL ERROR:" << err.text();
		//qfError() << ("QUERY: "%q.lastQuery());
		//::exit(-1);
	}
	return q;
}

void HtmlFileExporter::generateHtml()
{
	QDir html_dir(outDir());
	if(!html_dir.exists()) {
		qfInfo() << "creating HTML dir:" << outDir();
		if(!QDir().mkpath(outDir())) {
			qfError() << "Cannot create out dir:" << outDir();
			return;
		}
		html_dir = QDir(outDir());
		if(!html_dir.exists()) {
			qfError() << "Author even doesn't know, how to use QDir API.";
			return;
		}
	}
	QVariantMap event_info = eventInfo();
	//qfDebug() << event_info;
	//QString event_name = event_info.value("name").toString();
	//ui->lblHeadRight->setText(event_info.value("date").toString());
	//int stage_cnt = event_info.value("stageCount").toInt();
	int curr_stage = currentStage();
	if(curr_stage == 0) {
		curr_stage = event_info.value("currentStageId").toInt();
		if(curr_stage == 0)
			curr_stage = 1;
		qfInfo() << "Setting stage to:" << curr_stage;
		setCurrentStage(curr_stage);
	}

	QVariantList html_body = QVariantList() << QStringLiteral("body");
	html_body.insert(html_body.length(), QVariantList() << QStringLiteral("body"));

	exportClasses();
}

QString HtmlFileExporter::normalizeClassName(const QString class_name)
{
	QString ret = class_name;
	ret.replace(' ', '-');
	return QString::fromUtf8(qf::core::Collator::toAscii7(QLocale::Czech, ret, true));
}

void HtmlFileExporter::exportClasses()
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

	QVariantList class_links;
	QList<int> class_ids;
	QSqlQuery q = execSql(qs);
	while(q.next()) {
		int class_id = q.value("id").toInt();
		class_ids << class_id;
		QString class_name = q.value("name").toString();
		QString class_name_ascii7 = normalizeClassName(class_name);

		class_links.insert(class_links.length(), QVariantList{"a", QVariantMap{{"href", class_name_ascii7 + ".html"}}, class_name});
	}
	for(int class_id : class_ids) {
		exportClass(class_id, class_links);
	}

	QVariantMap event_info = eventInfo();
	QDir html_dir(outDir());
	{
		QString title = tr("E%1 %2").arg(currentStage()).arg(reportTitle());
		QVariantList html_body = QVariantList() << QStringLiteral("body");
		html_body.insert(html_body.length(), QVariantList{"h1", title});
		html_body.insert(html_body.length(), QVariantList{"h2", event_info.value("name")});
		html_body.insert(html_body.length(), QVariantList{"h3", event_info.value("place")});
		html_body.insert(html_body.length(), QVariantList{"p"} << class_links);
		//qfInfo() << html_body;
		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(title);
		QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
		QString sub_dir = QString("E%1/%2").arg(currentStage()).arg(reportDir());
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

}}}
