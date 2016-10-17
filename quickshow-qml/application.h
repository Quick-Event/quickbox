#ifndef APPLICATION_H
#define APPLICATION_H

#include <qf/core/utils.h>

#include <QGuiApplication>

class QVariant;
class QSqlDatabase;
class QSqlQuery;
class QSqlRecord;
class AppCliOptions;

class Model;

namespace qf {
	namespace core {
		namespace sql {
			class Connection;
		}
	}
}

class Application : public QGuiApplication
{
	Q_OBJECT

	Q_PROPERTY(bool sqlConnected READ isSqlConnected WRITE setSqlConnected NOTIFY sqlConnectedChanged)
private:
	typedef QGuiApplication Super;
public:
	explicit Application(int & argc, char ** argv, AppCliOptions *cli_opts);

	QF_PROPERTY_BOOL_IMPL(s, S, qlConnected)

	static Application *instance();
	AppCliOptions* cliOptions() {return m_cliOptions;}
	qf::core::sql::Connection sqlConnetion();
	QSqlQuery execSql(const QString &query_str);
	QVariantMap sqlRecordToMap(const QSqlRecord &rec);
public slots:
	QVariant eventInfo();
	QString profile();
	QVariant cliOptionValue(const QString &option_name);
	QObject* model();
protected:
	AppCliOptions *m_cliOptions;
	Model *m_model;
};

#endif // APPLICATION_H
