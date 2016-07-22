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

	QF_VARIANTMAP_FIELD(int, r, setR, unId)
	QF_VARIANTMAP_FIELD(int, c, setC, ourseId)
	QF_VARIANTMAP_FIELD(int, s, setS, tageStartTimeMs) //< stage start till midnight
	QF_VARIANTMAP_FIELD(int, C, setC, heckTimeMs) //< check time till stage start
	QF_VARIANTMAP_FIELD(int, s, setS, tartTimeMs) //< start time till stage start
	QF_VARIANTMAP_FIELD(int, f, setF, inishTimeMs) //< finish time till stage start
	//QF_VARIANTMAP_FIELD2(int, f, setF, inishLapTimeMs, 0) //< finish lap time till competitor start time
	//QF_VARIANTMAP_FIELD2(int, f, setF, inishStpTimeMs, 0) //< finish stp time till competitor start time
	QF_VARIANTMAP_FIELD(int, c, setC, ardNumber)
	QF_VARIANTMAP_FIELD(QVariantList, p, setP, unches) //< punch times including finish, all measured till competitor start time
	QF_VARIANTMAP_FIELD(bool, is, set, BadCheck)
	QF_VARIANTMAP_FIELD(bool, is, set, MisPunch)

public:
	CheckedCard(const QVariantMap &data = QVariantMap());

	int punchCount() const;
	CheckedPunch punchAt(int i) const;

	int timeMs() const;
	QString toString() const;

	bool isOk() const {return !isBadCheck() && !isMisPunch();}
};

}

#endif
