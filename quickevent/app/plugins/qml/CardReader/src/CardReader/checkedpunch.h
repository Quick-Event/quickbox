#ifndef CARDREADER_CHECKEDPUNCH_H
#define CARDREADER_CHECKEDPUNCH_H

#include "../cardreaderpluginglobal.h"

#include <qf/core/utils.h>

#include <QVariantMap>

namespace CardReader {

class CARDREADERPLUGIN_DECL_EXPORT CheckedPunch : public QVariantMap
{
	QF_VARIANTMAP_FIELD2(int, c, setC, ode, 0)
	QF_VARIANTMAP_FIELD2(int, p, setP, osition, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tpTimeMs, 0)
	QF_VARIANTMAP_FIELD2(int, l, setL, apTimeMs, 0)
public:
	CheckedPunch(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
};

} // namespace CardReader

#endif // CARDREADER_CHECKEDPUNCH_H
