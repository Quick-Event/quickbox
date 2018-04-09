#ifndef APPLICATION_H
#define APPLICATION_H

#include <qf/core/utils.h>

#include <QCoreApplication>

class QVariant;
class QSqlDatabase;
class QSqlQuery;
class QSqlRecord;
class AppCliOptions;
class QDir;

namespace qf {
	namespace core {
		namespace sql {
		class Connection;
		class Query;
		}
	}
}

class Application : public QCoreApplication
{
	Q_OBJECT
private:
	using Super = QCoreApplication;
public:
	explicit Application(int & argc, char ** argv, AppCliOptions *cli_opts);

	QF_PROPERTY_BOOL_IMPL(s, S, qlConnected)

	static Application *instance();
	AppCliOptions* cliOptions() {return m_cliOptions;}
	qf::core::sql::Connection sqlConnetion();
	qf::core::sql::Query execSql(const QString &query_str);
	//QVariantMap sqlRecordToMap(const QSqlRecord &rec);

	QVariantMap eventInfo();
private:
	void generateHtml();
	void exportClassesResults(const QDir &html_dir, int stage_no);
	void exportClassResults(const QDir &html_dir, int stage_no, int class_id, const QVariantList &class_links);

private:
	AppCliOptions *m_cliOptions;
};

#endif // APPLICATION_H
