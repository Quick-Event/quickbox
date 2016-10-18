#ifndef APPLICATION_H
#define APPLICATION_H

#include <qf/core/utils.h>

#include <QApplication>

class QVariant;
class QSqlDatabase;
class QSqlQuery;
class QSqlRecord;
class AppCliOptions;

namespace qf {
	namespace core {
		namespace sql {
		class Connection;
		class Query;
		}
	}
}

class Application : public QApplication
{
	Q_OBJECT

	//Q_PROPERTY(bool sqlConnected READ isSqlConnected WRITE setSqlConnected NOTIFY sqlConnectedChanged)
private:
	typedef QApplication Super;
public:
	explicit Application(int & argc, char ** argv, AppCliOptions *cli_opts);

	QF_PROPERTY_BOOL_IMPL(s, S, qlConnected)

	static Application *instance();
	AppCliOptions* cliOptions() {return m_cliOptions;}
	qf::core::sql::Connection sqlConnetion();
	qf::core::sql::Query execSql(const QString &query_str);
	QVariantMap sqlRecordToMap(const QSqlRecord &rec);
public slots:
	QVariant eventInfo();
	QString profile();
	QVariant cliOptionValue(const QString &option_name);
protected:
	AppCliOptions *m_cliOptions;
};

#endif // APPLICATION_H
