#ifndef QF_CORE_SQL_DBFSATTRS_H
#define QF_CORE_SQL_DBFSATTRS_H

#include "../core/coreglobal.h"
#include "../core/utils.h"

#include <QSharedDataPointer>

namespace qf {
namespace core {
namespace sql {

class DbFsAttrs
{
public:
	enum NodeType {Invalid, Dir, File};
public:
	DbFsAttrs();
	DbFsAttrs(NodeType t, bool deleted);
private:
	class SharedDummyHelper {};
	class Data : public QSharedData
	{
	public:
		NodeType type;
		bool isDeleted;
		//unsigned canUpdate:1, isPriKey:1, isSerial:1, isNullable:1;
		Data(NodeType t = Invalid, bool deleted = false) : type(t), isDeleted(deleted) {}
	};
	QSharedDataPointer<Data> d;

	DbFsAttrs(SharedDummyHelper);
	static const DbFsAttrs& sharedNull();
public:
	bool isNull() const {return d == sharedNull().d;}

	QF_SHARED_CLASS_FIELD_RW(NodeType, t, setT, ype)
	QF_SHARED_CLASS_FIELD_RW(bool, is, set, Deleted)
};

}}}

#endif // QF_CORE_SQL_DBFSATTRS_H
