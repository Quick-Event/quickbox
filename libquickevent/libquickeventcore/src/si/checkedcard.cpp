#include "checkedcard.h"

#include "../og/timems.h"

namespace quickevent {
namespace core {
namespace si {

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
		punch_lst << QString("[%1. %2: %3 %4 %5]")
					 .arg(++position)
					 .arg(p.code())
					 .arg(quickevent::core::og::TimeMs(p.stpTimeMs()).toString())
					 .arg(quickevent::core::og::TimeMs(p.lapTimeMs()).toString())
					 .arg(p.contains(QStringLiteral("distance"))? p.distance(): -1);
	}
	ret += QString("SI: %1, run_id: %2, course_id: %6, start: %3, finish: %4, punches: %5")
			.arg(cardNumber())
			.arg(runId())
			.arg(quickevent::core::og::TimeMs(startTimeMs()).toString())
			.arg(quickevent::core::og::TimeMs(finishTimeMs()).toString())
			.arg(punch_lst.join(", "))
			.arg(courseId());
	return ret;
}

}}}
