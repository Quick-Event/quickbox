#ifndef CARDREADER_CARD_H
#define CARDREADER_CARD_H

#include <qf/core/utils.h>

#include <QVariantMap>

namespace CardReader {

class CheckedCard : public QVariantMap
{
	QF_VARIANTMAP_FIELD(bool, is, set, Ok)
	QF_VARIANTMAP_FIELD(int, c, setC, ourseId)
	QF_VARIANTMAP_FIELD(int, s, setS, tartTimeMs)
	QF_VARIANTMAP_FIELD(int, f, setF, inishTimeMs)
	QF_VARIANTMAP_FIELD(int, l, setL, apTimeMs)
	QF_VARIANTMAP_FIELD(int, c, setC, ardNumber)
	QF_VARIANTMAP_FIELD(QVariantList, p, setP, unchList)
public:
	CheckedCard(const QVariantMap &data = QVariantMap());
};

}

#endif
