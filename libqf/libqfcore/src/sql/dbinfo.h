#ifndef QF_CORE_SQL_DBINFO_H
#define QF_CORE_SQL_DBINFO_H

#include "../core/coreglobal.h"

#include <QString>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlIndex>
#include <QSharedData>
#include <QStringList>

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT DbInfo : public QSqlDatabase
{
public:
	DbInfo();
	explicit DbInfo(const QSqlDatabase& qdb);
	//explicit DbInfo(const QString &driver_name);
	//explicit DbInfo(QSqlDriver *drv);
	//virtual ~DbInfo();
public:
	typedef QMap<QString, QString> ConnectionOptions;
	struct QFCORE_DECL_EXPORT IndexInfo {
		QString name;
		bool unique;
		bool primary;
		QStringList fields;

		IndexInfo() : unique(false), primary(false) {}
	};
	typedef QList<IndexInfo> IndexList;
public:
	//! pomocna funkce pro debugging
	//static QString cash2string(const QString &indent = QString());

	//! @return list of fields in table or view
	QStringList fields(const QString& tbl_name) const;

	/// @return list of available tables
	QStringList tables(const QString& dbname = QString::null, QSql::TableType type = QSql::Tables) const;

	/// @return list of indexes for table \a tbl_name .
	IndexList indexes(const QString& tbl_name) const;

	/// @return list of available databases
	QStringList databases() const;

	/// @return list of available schemas in current connection
	QStringList schemas() const;

	QSqlIndex primaryIndex(const QString& tblname);

	QSqlRecord record(const QString & tablename) const;

	/// @return kind of relname.
	/// \sa RelationKindKind
	//QFSql::RelationKind relationKind(const QString& relname);

	/**
		 * @return string unique per user,database_name,host,driver
		 */
	QString signature() const;
	//static QString signature2driverName(const QString &sig);
	//! Returns human readable textual information about current connection.
	QString info(int verbosity = 1) const;
	bool isOpen() const;
	/*
		/// if successfull, lastError() returns information about connection.
		void open(const ConnectionOptions &options = ConnectionOptions());
		void open(const QString & user, const QString & password, const ConnectionOptions &options = ConnectionOptions())
		{
			setUserName(user);
			setPassword(password);
			open(options);
		}
		void close();
		*/
	static int defaultPort(const QString &driver_name);
public:
	//QString currentSchema() const;
	void setCurrentSchema(const QString &schema_name);

	//! retrieves CREATE TABLE ... Sql script for \a tblname.
	QString createTableSqlCommand(const QString &tblname);

	//! retrieves INSERT INTO ... Sql script for \a tblname.
	QString dumpTableSqlCommand(const QString &tblname);

	QStringList serverVersion() const;
	QString fullTableNameToQtDriverTableName(const QString &full_table_name) const;

private:
	//! take CREATE TABLE ... and parse fields definitions from it.
	static QStringList fieldDefsFromCreateTableCommand(const QString &cmd);

	//! convert n to form .dbname.tblname.fldname
	QString normalizeFieldName(const QString &n) const;
	//! convert n to form .dbname.tblname (.schemaname.tblname for PSQL)
	QString normalizeTableName(const QString &n) const;
	//! if \a n is empty return name of default database/schema for active connection.
	QString normalizeDbName(const QString &n) const;
};

}}}

#endif // QF_CORE_SQL_DBINFO_H
