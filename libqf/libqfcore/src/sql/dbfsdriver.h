#ifndef QF_CORE_SQL_DBFSDRIVER_H
#define QF_CORE_SQL_DBFSDRIVER_H

#include "dbfsattrs.h"
#include "connection.h"

#include "../core/coreglobal.h"
#include "../core/utils.h"

#include <QObject>

namespace qf {
namespace core {
namespace sql {

class Query;

class QFCORE_DECL_EXPORT DbFsDriver : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged)
	Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
	Q_PROPERTY(QString snapshotsTableName READ snapshotsTableName WRITE setSnapshotsTableName NOTIFY snapshotsTableNameChanged)
	Q_PROPERTY(int snapshotNumber READ snapshotNumber WRITE setSnapshotNumber NOTIFY snapshotNumberChanged)
public:
	explicit DbFsDriver(QObject *parent = 0);
	~DbFsDriver() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, c, C, onnectionName)
	QF_PROPERTY_IMPL(QString, t, T, ableName)
	QF_PROPERTY_IMPL(QString, s, S, napshotsTableName)
	QF_PROPERTY_IMPL2(int, s, S, napshotNumber, 0)

	bool createSnapshot(const QString &comment);
	DbFsAttrs attributes(const QString &path);
	QList<DbFsAttrs> readDir(const QString &parent_path);
	QByteArray get(const QString &path, bool *pok = nullptr);
private:
	QString cleanPath(const QString &path);
	QString attributesColumns(const QString &table_alias);
	DbFsAttrs attributesFromQuery(const Query &q);
	Connection connection();
	bool initDbFs();
private:
	DbFsAttrs readAttrs(const QString &path, int pinode = 0);
	QList<DbFsAttrs> readDir(int parent_inode);
	QMap<QString, DbFsAttrs> m_attributeCache;
};

}}}

#endif // QF_CORE_SQL_DBFSDRIVER_H
