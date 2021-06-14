#ifndef QUICKEVENTCORE_SI_PUNCHRECORD_H
#define QUICKEVENTCORE_SI_PUNCHRECORD_H

#include "../quickeventcoreglobal.h"

#include <qf/core/utils.h>

#include <QVariantMap>

namespace qf { namespace core { namespace sql { class Query; }}}
//namespace siut { class SIPunch; }

namespace quickevent {
namespace core {
namespace si {

class QUICKEVENTCORE_DECL_EXPORT PunchRecord : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD(int, i, seti, d)
	QF_VARIANTMAP_FIELD(int, r, setr, unid)
	QF_VARIANTMAP_FIELD(int, s, sets, tageid)
	QF_VARIANTMAP_FIELD(int, s, sets, iid)
	QF_VARIANTMAP_FIELD(int, c, setc, ode)
	QF_VARIANTMAP_FIELD(int, t, sett, ime)
	QF_VARIANTMAP_FIELD(int, m, setm, sec)
	QF_VARIANTMAP_FIELD(int, t, sett, imems)
	QF_VARIANTMAP_FIELD(int, r, setr, untimems)
public:
	PunchRecord(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
	PunchRecord(const qf::core::sql::Query &q);

	QString toString() const;
};

}}}

#endif // QUICKEVENTCORE_SI_PUNCHRECORD_H
