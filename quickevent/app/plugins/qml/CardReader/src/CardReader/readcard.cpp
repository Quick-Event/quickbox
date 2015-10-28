#include "readcard.h"

#include <siut/simessage.h>

#include <QSqlRecord>
#include <QJsonDocument>

using namespace CardReader;

ReadPunch::ReadPunch(const QVariantList &var_list)
{
	setCode(var_list.value(0).toInt());
	setTime(var_list.value(1).toInt());
	setMsec(var_list.value(2).toInt());
	//setDay(var_list.value(3).toInt());
	//setWeek(var_list.value(4).toInt());
}

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

ReadCard::ReadCard(const QSqlRecord &rec)
{
	//setCardId(rec.value("cards.id").toInt());
	setRunId(rec.value("runId").toInt());
	setStationCodeNumber(rec.value("stationNumber").toInt());
	setCardNumber(rec.value("siId").toInt());
	setCheckTime(rec.value("checkTime").toInt());
	setStartTime(rec.value("startTime").toInt());
	setFinishTime(rec.value("finishTime").toInt());
	setFinishTimeMs(0);
	auto jsd = QJsonDocument::fromJson(rec.value("punches").toString().toUtf8());
	QVariantList punchlst;
	QVariantList lst = jsd.toVariant().toList();
	for(auto v : lst) {
		ReadPunch punch(v.toList());
		punchlst << punch;
	}
	this->setPunches(punchlst);
}

ReadCard::ReadCard(const SIMessageCardReadOut &si_card)
	: Super(si_card.toVariantMap())
{
}

PunchRecord::PunchRecord(const SIMessageTransmitRecord &rec)
	: Super(rec.toVariantMap())
{
}
