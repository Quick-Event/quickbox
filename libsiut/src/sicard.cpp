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

SICard::SICard()
{
}

SICard::SICard(int card_number)
{
	setCardNumber(card_number);
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
	for (int n = 0; n < punchCount(); ++n) {
		SIPunch p = punchAt(n);
		sl << ("   " + QString::number(n+1)).right(4) + ".\t" + QString::number(p.code()) + "\t" + time_str(p.time());
	}
	return sl.join("\n");
}

int SICard::punchCount() const
{
	return punches().count();
}

SIPunch SICard::punchAt(int i) const
{
	QVariantMap m = punches().value(i).toMap();
	return SIPunch(m);
}

QList<SIPunch> SICard::punchList() const
{
	QList<SIPunch> ret;
	for (int i = 0; i < punchCount(); ++i) {
		ret << punchAt(i);
	}
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
