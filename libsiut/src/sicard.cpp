#include "sicard.h"

namespace siut {

static QString time_str(int _time)
{
	QString ret = "%1:%2:%3";
	if(_time == 0xEEEE) ret = "----";
	else {
		int time = SICard::toAM(_time);
		ret = ret.arg(time / (60*60)).arg(QString::number((time / 60) % 60), 2, '0').arg(QString::number(time % 60), 2, '0');
	}
	return ret;
}
/*
static QString ob_time_str(int _time)
{
	QString ret = "%1.%2";
	if(_time == 0xEEEE) ret = "----";
	else {
		int time = SICard::toAM(_time);
		ret = ret.arg(time / 60).arg(QString::number(time % 60), 2, '0');
	}
	return ret;
}
*/
const SICard &SICard::sharedNull()
{
	static SICard n = SICard(SharedDummyHelper());
	return n;
}

SICard::SICard(SICard::SharedDummyHelper)
{
	d = new Data();
}

SICard::SICard()
{
	*this = sharedNull();
}

SICard::SICard(int card_number)
{
	d = new Data;
	d->cardNumber = card_number;
}

QString SICard::toString() const
{
	QStringList sl;
	//sl << data().dump();
	//sl << tr("cardType: %1 (raw type: %2)").arg(cardTypeToString(cardType())).arg(rawCardType());
	sl << tr("stationNumber: %1").arg(stationNumber());
	sl << tr("cardNumber: %1").arg(cardNumber());
	//sl << tr("startNumber: %1").arg(startNumber());
	//sl << tr("countryCode: %1").arg(countryCode());
	//sl << tr("clubCode: %1").arg(clubCode());
	sl << tr("check: %1").arg(time_str(checkTime()));
	sl << tr("start: %1").arg(time_str(startTime()));
	sl << tr("finish: %1").arg(time_str(finishTime()));
	int n = 0;
	for(const SIPunch &p : punches())
		sl << QString::number(++n) + ".\t" + QString::number(p.code()) + "\t" + time_str(p.time());
	return sl.join("\n");
}

QVariantMap SICard::toVariantMap() const
{
	QVariantMap ret;// = SIMessageBase::toVariant().toMap();
	ret[QStringLiteral("stationNumber")] = stationNumber();
	ret[QStringLiteral("cardNumber")] = cardNumber();
	//ret["cardNumberFull"] = (cardNumber() < 100000)? 100000 + cardNumber(): cardNumber();
	//ret["startNumber"] = startNumber();
	//ret["countryCode"] = countryCode();
	//ret["clubCode"] = clubCode();
	ret[QStringLiteral("checkTime")] = checkTime();
	ret[QStringLiteral("startTime")] = startTime();
	ret[QStringLiteral("finishTime")] = finishTime();
	ret[QStringLiteral("finishTimeMs")] = 0; // TODO: some cards supports msecs, read it
	QVariantList punch_list;
	foreach(const SIPunch &p, punches()) {
		punch_list << p.toVariantMap();
	}
	ret["punches"] = punch_list;
	return ret;
}

int SICard::toAMms(int time_msec)
{
	constexpr int MSEC_12HR = 12 * 60 * 60 * 1000;
	int ret = time_msec;
	while(ret < 0)
		ret += MSEC_12HR;
	while(ret >= MSEC_12HR)
		ret -= MSEC_12HR;
	return ret;
}

int SICard::toAM(int time_sec)
{
	return toAMms(time_sec * 1000) / 1000;
}

}
