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
public:
	enum PutNodeOptions {PN_CREATE = 1, PN_TRUNCATE = 2, PN_OVERRIDE = 4, PN_DELETE = 8, PN_RENAME = 16};
	static const QString CHANNEL_INVALIDATE_DBFS_DRIVER_CACHE;
private:
	enum CacheRemoveMode {CRM_Noop, CRM_Single, CRM_Recursive};
public:
	explicit DbFsDriver(QObject *parent = 0);
	~DbFsDriver() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, c, C, onnectionName)
	QF_PROPERTY_IMPL2(QString, t, T, ableName, QStringLiteral("dbfs"))

	bool checkDbFs();
	bool createDbFs();
	DbFsAttrs attributes(const QString &path);
	QList<DbFsAttrs> childAttributes(const QString &parent_path);
	QByteArray get(const QString &path, bool *pok = nullptr);
	bool put(const QString &path, const QByteArray &data, bool create_if_not_exist = false);
	/// create all the necessarry directories and file
	bool putmkdir(const QString &path, const QByteArray &data);
	bool truncate(const QString &path, int new_size);
	bool mkfile(const QString &path, const QByteArray &data = QByteArray());
	bool mkdir(const QString &path);
	bool mkdirs(const QString &path);
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

	static QString cacheRemoveModeToString(CacheRemoveMode opt);
	static CacheRemoveMode cacheRemoveModeFromString(const QString &str);
	template <class T>
	void cacheRemove_helper(T &map, const QString &path, DbFsDriver::CacheRemoveMode mode);
	void cacheRemove(const QString &file_path, CacheRemoveMode file_mode, const QString &dir_path, CacheRemoveMode dir_mode, bool post_notify);
	void postAttributesChangedNotify(const QString &pay_load);
	Q_SLOT void onSqlNotify(const QString &channel, QSqlDriver::NotificationSource source, const QVariant &payload);

	DbFsAttrs sqlSelectNode(int inode, QByteArray *pdata = nullptr);
	DbFsAttrs sqlInsertNode(const DbFsAttrs &attrs, const QByteArray &data);
	bool sqlDeleteNode(int inode);
	bool sqlUpdateNode(int inode, const QByteArray &data);
	bool sqlRenameNode(int inode, const QString &new_name);

	DbFsAttrs readAttrs(const QString &spath, int pinode);
	QList<DbFsAttrs> readChildAttrs(int parent_inode);
private:
	typedef QMap<QString, DbFsAttrs> FileAttributesCache;
	FileAttributesCache m_fileAttributesCache;
	typedef QMap<QString, QStringList> DirectoryCache;
	DirectoryCache m_directoryCache;
	int m_latestSnapshotNumber = -1;
	//bool m_isNotifyRegistered = false;
};

}}}

#endif // QF_CORE_SQL_DBFSDRIVER_H
