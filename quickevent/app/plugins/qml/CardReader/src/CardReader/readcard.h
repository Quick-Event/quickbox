#ifndef CARDREADER_READCARD_H
#define CARDREADER_READCARD_H

#include <qf/core/utils.h>

#include <QVariantMap>

class QSqlRecord;
class SIMessageCardReadOut;

namespace CardReader {

class ReadPunch : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD2(int, c, setC, ode, 0)
	QF_VARIANTMAP_FIELD2(int, t, setT, ime, 0)
	QF_VARIANTMAP_FIELD2(int, m, setM, sec, 0)
	QF_VARIANTMAP_FIELD2(int, d, setD, ay, 0)
	QF_VARIANTMAP_FIELD2(int, w, setW, eek, 0)
public:
	ReadPunch(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
	ReadPunch(const QVariantList &var_list);

	QVariantList toVariantList() const;
	QString toJsonArrayString() const;
};

class ReadCard : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD2(int, r, setR, unId, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tationCodeNumber, 0)
	QF_VARIANTMAP_FIELD2(int, c, setC, ardNumber, 0)
	QF_VARIANTMAP_FIELD2(int, c, setC, heckTime, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartTime, 0)
	QF_VARIANTMAP_FIELD2(int, f, setF, inishTime, 0)
	QF_VARIANTMAP_FIELD2(int, f, setF, inishTimeMs, 0)
	QF_VARIANTMAP_FIELD(QVariantList, p, setP, unches)
public:
	ReadCard(const QVariantMap &data = QVariantMap());
	ReadCard(const QSqlRecord &rec);
	ReadCard(const SIMessageCardReadOut &si_card);
};

} // namespace CardReader

#endif // CARDREADER_READCARD_H
