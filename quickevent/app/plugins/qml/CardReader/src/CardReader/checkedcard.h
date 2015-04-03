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
	QF_VARIANTMAP_FIELD(bool, is, set, Ok)
	QF_VARIANTMAP_FIELD(int, c, setC, ourseId)
	QF_VARIANTMAP_FIELD(int, s, setS, tartTimeMs)
	QF_VARIANTMAP_FIELD(int, f, setF, inishTimeMs)
	QF_VARIANTMAP_FIELD(int, c, setC, ardNumber)
	QF_VARIANTMAP_FIELD(QVariantList, p, setP, unches)

public:
	CheckedCard(const QVariantMap &data = QVariantMap());

	int lapTimeMs() const {return finishTimeMs() - startTimeMs();}
};

}

#endif
