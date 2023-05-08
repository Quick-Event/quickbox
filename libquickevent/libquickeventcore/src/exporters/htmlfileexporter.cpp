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
	: Super(parent)
{
}

void HtmlFileExporter::generateHtml()
{
	prepareExport();
	
	exportClasses();
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
