#ifndef CARDREADER_CARD_H
#define CARDREADER_CARD_H

#include "../cardreaderpluginglobal.h"
#include "checkedpunch.h"

#include <qf/core/utils.h>

#include <QVariantMap>

namespace CardReader {

class CARDREADERPLUGIN_DECL_EXPORT CheckedCard : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD2(int, r, setR, unId, 0)
	QF_VARIANTMAP_FIELD2(bool, is, set, Ok, true)
	QF_VARIANTMAP_FIELD2(int, c, setC, ourseId, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tageStartTimeMs, 0) //< stage start till midnight
	QF_VARIANTMAP_FIELD2(int, C, setC, heckTimeMs, 0) //< check time till stage start
	QF_VARIANTMAP_FIELD2(int, s, setS, tartTimeMs, 0) //< start time till stage start
	QF_VARIANTMAP_FIELD2(int, f, setF, inishTimeMs, 0) //< fimish time till stage start
	QF_VARIANTMAP_FIELD2(int, f, setF, inishLapTimeMs, 0) //< finish lap time till competitor start time
	QF_VARIANTMAP_FIELD2(int, f, setF, inishStpTimeMs, 0) //< finish stp time till competitor start time
	QF_VARIANTMAP_FIELD2(int, c, setC, ardNumber, 0)
	QF_VARIANTMAP_FIELD(QVariantList, p, setP, unches) //< punch times are measured till competitor start time

public:
	CheckedCard(const QVariantMap &data = QVariantMap());

	int timeMs() const {return finishStpTimeMs();}
};

}

#endif
