#ifndef QF_CORE_SQL_TABLELOCKER_H
#define QF_CORE_SQL_TABLELOCKER_H

#include "transaction.h"

#include "../core/coreglobal.h"

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT TableLocker : public Transaction
{
public:
	TableLocker(const Connection &conn, const QString &table_name, const QString &lock_type);
	~TableLocker() Q_DECL_OVERRIDE;

	void lock();
private:
	QString m_tableName;
	QString m_lockType;
};

}}}

#endif // TABLELOCKER_H
