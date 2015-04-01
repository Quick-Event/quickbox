#ifndef CARDREADER_CHECKEDPUNCH_H
#define CARDREADER_CHECKEDPUNCH_H

#include <qf/core/utils.h>

#include <QVariantMap>

namespace CardReader {

class CheckedPunch : public QVariantMap
{
	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(int, p, setP, osition)
	QF_VARIANTMAP_FIELD(int, s, setS, tpTimeMs)
	QF_VARIANTMAP_FIELD(int, l, setL, apTimeMs)
public:
	CheckedPunch(const QVariantMap &data = QVariantMap());
};

} // namespace CardReader

#endif // CARDREADER_CHECKEDPUNCH_H
