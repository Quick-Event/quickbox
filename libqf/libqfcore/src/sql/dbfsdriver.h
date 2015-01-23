#ifndef QF_CORE_SQL_DBFSDRIVER_H
#define QF_CORE_SQL_DBFSDRIVER_H

#include "../core/coreglobal.h"
#include "../core/utils.h"

#include <QObject>

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT DbFsDriver : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged)
	Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
	Q_PROPERTY(QString snapshotsTableName READ snapshotsTableName WRITE setSnapshotsTableName NOTIFY snapshotsTableNameChanged)
public:
	explicit DbFsDriver(QObject *parent = 0);
	~DbFsDriver() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, c, C, onnectionName)
	QF_PROPERTY_IMPL(QString, t, T, ableName)
	QF_PROPERTY_IMPL(QString, s, S, napshotsTableName)
private:
};

}}}

#endif // QF_CORE_SQL_DBFSDRIVER_H
