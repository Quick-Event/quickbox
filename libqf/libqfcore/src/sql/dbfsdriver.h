#ifndef QF_CORE_SQL_DBFSDRIVER_H
#define QF_CORE_SQL_DBFSDRIVER_H

#include "dbfsattrs.h"
#include "connection.h"

#include "../core/coreglobal.h"
#include "../core/utils.h"
#include "../utils/table.h"

#include <QObject>
#include <QSqlDriver>

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
	enum PutNodeOptions {PN_CREATE = 1, PN_TRUNCATE = 2, PN_OVERRIDE = 4, PN_DELETE = 8, PN_RENAME = 16};
public:
	explicit DbFsDriver(QObject *parent = 0);
	~DbFsDriver() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, c, C, onnectionName)
	QF_PROPERTY_IMPL(QString, t, T, ableName)
	QF_PROPERTY_IMPL2(int, s, S, napshotNumber, std::numeric_limits<int32_t>::max())

	bool createDbFs();
	DbFsAttrs attributes(const QString &path);
	QList<DbFsAttrs> childAttributes(const QString &parent_path);
	QByteArray get(const QString &path, bool *pok = nullptr);
	bool put(const QString &path, const QByteArray &data, bool create_if_not_exist = false);
	bool truncate(const QString &path, int new_size);
	bool mkfile(const QString &path, const QByteArray &data = QByteArray());
	bool mkdir(const QString &path);
	bool rmnod(const QString &path);
	bool rename(const QString &old_path, const QString &new_path);

	static QPair<QString, QString> splitPathFile(const QString &path);
	static QStringList splitPath(const QString &path);
	static QString joinPath(const QString &p1, const QString &p2);
	static QString cleanPath(const QString &path);
private:
	static QString attributesColumns(const QString &table_alias = QString());
	DbFsAttrs attributesFromQuery(const Query &q);
	Connection connection();

	bool checkWritePermissions();
	bool mknod(const QString &path, DbFsAttrs::NodeType node_type, const QByteArray &data);
	DbFsAttrs put_helper(const QString &spath, const DbFsAttrs &new_attrs, const QByteArray &data, int options, int new_size);

	void cacheRemove(const QString &path, bool post_notify);
	void postAttributesChangedNotify(const QString &path);
	Q_SLOT void onSqlNotify(const QString &channel, QSqlDriver::NotificationSource source, const QVariant payload);

	DbFsAttrs sqlSelectNode(int inode, QByteArray *pdata = nullptr);
	DbFsAttrs sqlInsertNode(const DbFsAttrs &attrs, const QByteArray &data);
	bool sqlDeleteNode(int inode);
	bool sqlUpdateNode(int inode, const QByteArray &data);
	bool sqlRenameNode(int inode, const QString &new_name);

	DbFsAttrs readAttrs(const QString &spath, int pinode);
	QList<DbFsAttrs> readChildAttrs(int parent_inode);
private:
	typedef QMap<QString, DbFsAttrs> AttributesCache;
	AttributesCache m_attributeCache;
	typedef QMap<QString, QStringList> DirectoryCache;
	DirectoryCache m_directoryCache;
	int m_latestSnapshotNumber = -1;
	//bool m_isNotifyRegistered = false;
};

}}}

#endif // QF_CORE_SQL_DBFSDRIVER_H
