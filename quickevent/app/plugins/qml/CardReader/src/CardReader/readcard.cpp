#include "readcard.h"

#include <siut/simessage.h>

#include <QSqlRecord>
#include <QJsonDocument>

using namespace CardReader;

QVariantList ReadPunch::toVariantList() const
{
	return QVariantList() << code() << time() << msec();// << dayOfWeek() << weekCnt();
}

QString ReadPunch::toJsonArrayString() const
{
	QStringList sl;
	for(auto v : toVariantList()) {
		sl << v.toString();
	}
	return '[' + sl.join(", ") + ']';
}
/*
ReadPunch ReadPunch::fromJsonArrayString(const QString &array_str)
{

}
*/
ReadCard::ReadCard(const QSqlRecord &rec)
{
	this->setRunId(rec.value("runId").toInt());
	this->setStationCodeNumber(rec.value("stationNumber").toInt());
	this->setCardNumber(rec.value("siId").toInt());
	this->setCheckTime(rec.value("checkTime").toInt());
	this->setStartTime(rec.value("startTime").toInt());
	this->setFinishTime(rec.value("finishTime").toInt());
	auto jsd = QJsonDocument::fromJson(rec.value("punches").toString().toUtf8());
	QVariantList punchlst;
	QVariantList lst = jsd.toVariant().toList();
	for(auto v : lst) {
		QVariantMap m = v.toMap();
		ReadPunch punch(m);
		punchlst << punch;
	}
	this->setPunches(punchlst);
}

ReadCard::ReadCard(const SIMessageCardReadOut &si_card)
	: Super(si_card.toVariant())
{
}
