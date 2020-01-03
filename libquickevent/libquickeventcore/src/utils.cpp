#include "utils.h"

#include <QDateTime>

namespace quickevent {
namespace core {

 QString Utils::dateTimeToIsoStringWithUtcOffset(const QDateTime &dt)
{
	QString ret = dt.toString(Qt::ISODate);
	if(dt.timeSpec() == Qt::LocalTime) {
		int offset_min = dt.offsetFromUtc();
		if(offset_min == 0) {
			ret += 'Z';
		}
		else {
			if(offset_min < 0) {
				ret += '-';
				offset_min = -offset_min;
			}
			else {
				ret += '+';
			}
			ret += QStringLiteral("%1:%2").arg(offset_min / (60 * 60), 2, 10, QChar('0')).arg((offset_min / 60) % 60, 2, 10, QChar('0'));
		}
	}
	return ret;
}

} // namespace core
} // namespace quickevent
