#ifndef SIUT_SIPUNCH_H
#define SIUT_SIPUNCH_H

#include <siut/siutglobal.h>

#include <qf/core/utils.h>

#include <QSharedDataPointer>
#include <QVariantMap>

namespace siut {

class SIUT_DECL_EXPORT SIPunch
{
public:
	enum DayOfWeek {Sunday = 0, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};
private:
	class SharedDummyHelper {};
	struct Data : public QSharedData
	{
		int stationNumber = 0;
		bool pmFlag = false;
		int code = 0;
		uint16_t time = 0;
		int msec = 0;
		DayOfWeek dayOfWeek = Sunday;
		int weekCnt = 0;
	};
	QSharedDataPointer<Data> d;

	SIPunch(SharedDummyHelper); /// null
	static const SIPunch& sharedNull();
public:

	SIPunch();
	SIPunch(int code, int time);
	SIPunch(const QByteArray &card_data, int ix);

	bool isNull() const {return d == sharedNull().d;}

	QF_SHARED_CLASS_FIELD_RW(int, s, setS, tationNumber)
	QF_SHARED_CLASS_FIELD_RW(bool, p, setP, mFlag)
	QF_SHARED_CLASS_FIELD_RW(int, c, setC, ode)
	QF_SHARED_CLASS_FIELD_RW(uint16_t, t, setT, ime)
	QF_SHARED_CLASS_FIELD_RW(int, m, setM, sec)
	QF_SHARED_CLASS_FIELD_RW(DayOfWeek, d, setD, ayOfWeek)
	QF_SHARED_CLASS_FIELD_RW(int, w, setW, eekCnt)

	QVariantMap toVariantMap() const;
	QString toString() const;

	static unsigned getUnsigned(const QByteArray &ba, int ix, int byte_cnt = 2);
};

} // namespace siut

Q_DECLARE_METATYPE(siut::SIPunch)

#endif // SIPUNCH_H
