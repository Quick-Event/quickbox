#ifndef SIUT_SIPUNCH_H
#define SIUT_SIPUNCH_H

#include <siut/siutglobal.h>

#include <qf/core/utils.h>

#include <QSharedDataPointer>
#include <QVariantMap>

namespace siut {

class SIUT_DECL_EXPORT SIPunch : public QVariantMap
{
	using Super = QVariantMap;
public:
	enum DayOfWeek {Sunday = 0, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};
public:
	SIPunch();
	SIPunch(const QVariantMap &o) : Super(o) {}
	SIPunch(int code, int time);
	SIPunch(const QByteArray &card_data, int ix);

	QF_VARIANTMAP_FIELD(int, c, setC, ardNumber)
	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(int, t, setT, ime)
	QF_VARIANTMAP_FIELD(int, m, setM, sec)
	QF_VARIANTMAP_FIELD(bool, p, setP, mFlag)
	QF_VARIANTMAP_FIELD(int, d, setD, ayOfWeek)
	QF_VARIANTMAP_FIELD(int, w, setW, eekCnt)

//	QString toString() const; - undefined

	static unsigned getUnsigned(const QByteArray &ba, int ix, int byte_cnt = 2);
};

} // namespace siut

//Q_DECLARE_METATYPE(siut::SIPunch)

#endif // SIPUNCH_H
