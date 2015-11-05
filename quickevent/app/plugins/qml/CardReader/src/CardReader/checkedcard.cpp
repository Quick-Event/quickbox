#include "checkedcard.h"

#include <quickevent/og/timems.h>

using namespace CardReader;

CheckedCard::CheckedCard(const QVariantMap &data)
	: QVariantMap(data)
{

}

QString CheckedCard::toString() const
{
	QString ret;
	QStringList punch_lst;
	for(auto v : punches()) {
		CheckedPunch p(v.toMap());
		punch_lst << QString("[%1(%2) %3 %4]")
					 .arg(p.position())
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
