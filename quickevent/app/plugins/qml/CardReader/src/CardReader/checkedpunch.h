#ifndef CARDREADER_CHECKEDPUNCH_H
#define CARDREADER_CHECKEDPUNCH_H

#include "../cardreaderpluginglobal.h"

#include <qf/core/utils.h>

#include <QVariantMap>

namespace CardReader {

class CARDREADERPLUGIN_DECL_EXPORT CheckedPunch : public QVariantMap
{
	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(int, p, setP, osition)
	QF_VARIANTMAP_FIELD(int, s, setS, tpTimeMs)
	QF_VARIANTMAP_FIELD(int, l, setL, apTimeMs)
public:
	CheckedPunch(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
};

} // namespace CardReader

#endif // CARDREADER_CHECKEDPUNCH_H
