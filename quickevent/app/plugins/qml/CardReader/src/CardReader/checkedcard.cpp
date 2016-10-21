#include "checkedcard.h"

#include <quickevent/og/timems.h>

using namespace CardReader;

CheckedCard::CheckedCard(const QVariantMap &data)
	: QVariantMap(data)
{

}

int CheckedCard::punchCount() const
{
	return punches().count();
}

CheckedPunch CheckedCard::punchAt(int i) const
{
	QVariantMap m = punches().value(i).toMap();
	return CheckedPunch(m);
}

int CheckedCard::timeMs() const
{
	QVariant v = punches().value(punches().count()  -1);
	if(v.isValid()) {
		CheckedPunch p(v.toMap());
		return p.stpTimeMs();
	}
	return 0;
}

QString CheckedCard::toString() const
{
	QString ret;
	QStringList punch_lst;
	int position = 0;
	for(auto v : punches()) {
		CheckedPunch p(v.toMap());
		punch_lst << QString("[%1. %2: %3 %4]")
					 .arg(++position)
					 .arg(p.code())
					 .arg(quickevent::og::TimeMs(p.stpTimeMs()).toString())
					 .arg(quickevent::og::TimeMs(p.lapTimeMs()).toString());
	}
	ret += QString("SI: %1, run_id: %2, start: %3, finish: %4, punches: %5")
			.arg(cardNumber())
			.arg(runId())
			.arg(quickevent::og::TimeMs(startTimeMs()).toString())
			.arg(quickevent::og::TimeMs(finishTimeMs()).toString())
			.arg(punch_lst.join(", "));
	return ret;
}
