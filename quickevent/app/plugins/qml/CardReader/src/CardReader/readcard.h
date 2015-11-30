#ifndef CARDREADER_READCARD_H
#define CARDREADER_READCARD_H

#include <qf/core/utils.h>

#include <QVariantMap>

class QSqlRecord;
class SIMessageCardReadOut;
class SIMessageTransmitRecord;

namespace CardReader {

class ReadPunch : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(int, t, setT, ime)
	QF_VARIANTMAP_FIELD(int, m, setM, sec)
	QF_VARIANTMAP_FIELD(int, d, setD, ay)
	QF_VARIANTMAP_FIELD(int, w, setW, eek)
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

	QF_VARIANTMAP_FIELD(int, r, setR, unId)
	QF_VARIANTMAP_FIELD(int, s, setS, tationCodeNumber)
	QF_VARIANTMAP_FIELD(int, c, setC, ardNumber)
	QF_VARIANTMAP_FIELD(int, c, setC, heckTime)
	QF_VARIANTMAP_FIELD(int, s, setS, tartTime)
	QF_VARIANTMAP_FIELD(int, f, setF, inishTime)
	QF_VARIANTMAP_FIELD(int, f, setF, inishTimeMs)
	QF_VARIANTMAP_FIELD(QVariantList, p, setP, unches)
public:
	ReadCard(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
	ReadCard(const QSqlRecord &rec);
	ReadCard(const SIMessageCardReadOut &si_card);
	QString toString() const;
};


class PunchRecord : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD(int, r, setR, unId)
	QF_VARIANTMAP_FIELD(int, c, setC, ardNumber)
	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(int, t, setT, ime)
	QF_VARIANTMAP_FIELD(int, m, setM, sec)
	QF_VARIANTMAP_FIELD(int, d, setD, ay)
	QF_VARIANTMAP_FIELD(int, w, setW, eek)
public:
	PunchRecord(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
	PunchRecord(const SIMessageTransmitRecord &rec);

	//QVariantList toVariantList() const;
	//QString toJsonArrayString() const;
};

} // namespace CardReader

#endif // CARDREADER_READCARD_H
