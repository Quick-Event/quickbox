#ifndef OPENFILE_H
#define OPENFILE_H

#include <qf/core/utils.h>
#include <qf/core/sql/dbfsattrs.h>

#include <QSharedDataPointer>
#include <QIODevice>

class OpenFile
{
public:
	OpenFile();
	OpenFile(const qf::core::sql::DbFsAttrs &attrs);
private:
	class SharedDummyHelper {};
	class Data : public QSharedData
	{
	public:
		qf::core::sql::DbFsAttrs attrs;
		QByteArray data;
		bool isDataLoaded = false;
		bool isDataDirty = false;
		QIODevice::OpenMode openMode = 0;

		explicit Data() {}
		explicit Data(const qf::core::sql::DbFsAttrs &a) : attrs(a) {}
	};
	QSharedDataPointer<Data> d;

	OpenFile(SharedDummyHelper);
	static const OpenFile& sharedNull();
	QF_SHARED_CLASS_FIELD_RW(qf::core::sql::DbFsAttrs, a, setA, ttrs)
	QF_SHARED_CLASS_FIELD_RW(QByteArray, d, setD, ata)
	QF_SHARED_CLASS_FIELD_RW(bool, is, set, DataLoaded)
	QF_SHARED_CLASS_FIELD_RW(bool, is, set, DataDirty)
	QF_SHARED_CLASS_FIELD_RW(QIODevice::OpenMode, o, setO, penMode)
public:
	bool isNull() const {return d == sharedNull().d;}

	QByteArray& dataRef() {return d->data;}
};

#endif // OPENFILE_H
