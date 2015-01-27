#ifndef QF_CORE_SQL_DBFSDRIVER_H
#define QF_CORE_SQL_DBFSDRIVER_H

#include "dbfsattrs.h"
#include "connection.h"

#include "../core/coreglobal.h"
#include "../core/utils.h"
#include "../utils/table.h"

#include <QObject>

#include <limits>

namespace qf {
namespace core {
namespace sql {

class Query;

class QFCORE_DECL_EXPORT DbFsDriver : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged)
	Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
	Q_PROPERTY(int snapshotNumber READ snapshotNumber WRITE setSnapshotNumber NOTIFY snapshotNumberChanged)
public:
	explicit DbFsDriver(QObject *parent = 0);
	~DbFsDriver() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, c, C, onnectionName)
	QF_PROPERTY_IMPL(QString, t, T, ableName)
	QF_PROPERTY_IMPL2(int, s, S, napshotNumber, std::numeric_limits<int32_t>::max())

	bool createDbFs();
	bool createSnapshot(const QString &comment);
	qf::core::utils::Table listSnapshots();
	int latestSnapshotNumber();
	DbFsAttrs attributes(const QString &path);
	QList<DbFsAttrs> childAttributes(const QString &parent_path);
	QByteArray get(const QString &path, bool *pok = nullptr);
private:
	static QStringList splitPath(const QString &path);
	static QString joinPath(const QString &p1, const QString &p2);
	static QString cleanPath(const QString &path);
	static QString attributesColumns(const QString &table_alias = QString());
	DbFsAttrs attributesFromQuery(const Query &q);
	Connection connection();
	QString snapshotsTableName();
private:
	DbFsAttrs readAttrs(const QString &path, int pinode = 0);
	QList<DbFsAttrs> childAttributes(int parent_inode);
	QMap<QString, DbFsAttrs> m_attributeCache;
	QMap<QString, QStringList> m_directoryCache;
};

}}}

#endif // QF_CORE_SQL_DBFSDRIVER_H
