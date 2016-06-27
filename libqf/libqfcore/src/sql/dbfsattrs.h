#ifndef QF_CORE_SQL_DBFSATTRS_H
#define QF_CORE_SQL_DBFSATTRS_H

#include "../core/coreglobal.h"
#include "../core/utils.h"

#include <QSharedDataPointer>
#include <QDateTime>

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT DbFsAttrs
{
public:
	enum NodeType {Invalid, Dir, File};
public:
	DbFsAttrs();
	DbFsAttrs(NodeType t);
private:
	class SharedDummyHelper {};
	class Data : public QSharedData
	{
	public:
		int inode = -1;
		int pinode = -1;
		QDateTime mtime;
		NodeType type = Invalid;
		QString name;
		int size = 0;
		explicit Data(NodeType t) : type(t) {}
	};
	QSharedDataPointer<Data> d;

	DbFsAttrs(SharedDummyHelper);
	static const DbFsAttrs& sharedNull();
	QF_SHARED_CLASS_FIELD_RW(int, i, setI, node)
	QF_SHARED_CLASS_FIELD_RW(int, p, setP, inode)
	QF_SHARED_CLASS_FIELD_RW(NodeType, t, setT, ype)
	QF_SHARED_CLASS_FIELD_RW(QString, n, setN, ame)
	QF_SHARED_CLASS_FIELD_RW(int, s, setS, ize)
	QF_SHARED_CLASS_FIELD_RW(QDateTime, m, setM, time)
public:
	bool isNull() const {return d == sharedNull().d;}
	QChar typeChar() const;
	QString typeName() const;
	QString toString() const;
};

}}}

#endif // QF_CORE_SQL_DBFSATTRS_H
