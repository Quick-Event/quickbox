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

	static const bool O_RECURSIVE = true;

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
	bool put(const QString &path, const QByteArray &data);
	bool mkfile(const QString &path);
	bool rmfile(const QString &path);
	bool mkdir(const QString &path, bool o_recursive = !O_RECURSIVE);
	bool rmdir(const QString &path, bool o_recursive = !O_RECURSIVE);
private:
	static QPair<QString, QString> splitPathFile(const QString &path);
	static QStringList splitPath(const QString &path);
	static QString joinPath(const QString &p1, const QString &p2);
	static QString cleanPath(const QString &path);
	static QString attributesColumns(const QString &table_alias = QString());
	DbFsAttrs attributesFromQuery(const Query &q);
	Connection connection();
	QString snapshotsTableName();
	//bool mknod(int pinode, const QString &name, DbFsAttrs::NodeType type);

	bool rmnod(const QString &spath, bool o_recursive);
	/// @return attribudes of touched node
	DbFsAttrs touch(const DbFsAttrs &attrs, bool create_node, bool delete_node, const QByteArray &data);
	DbFsAttrs sqlInsertNode(const DbFsAttrs &attrs, const QByteArray &data);
	DbFsAttrs sqlUpdateNode(const DbFsAttrs &attrs, const QByteArray &data);

	DbFsAttrs readAttrs(const QString &path, int pinode = 0);
	QList<DbFsAttrs> readAttributes(int parent_inode);
	int readLatestSnapshotNumber();
private:
	QMap<QString, DbFsAttrs> m_attributeCache;
	QMap<QString, QStringList> m_directoryCache;
	int m_latestSnapshotNumber = -1;
};

}}}

#endif // QF_CORE_SQL_DBFSDRIVER_H
