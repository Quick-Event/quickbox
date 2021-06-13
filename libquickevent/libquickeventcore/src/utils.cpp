#include "utils.h"

#include <QDateTime>

namespace quickevent {
namespace core {

 QString Utils::dateTimeToIsoStringWithUtcOffset(QDateTime dt)
{
	dt.setOffsetFromUtc(dt.offsetFromUtc());
	return dt.toString(Qt::ISODate);
}

} // namespace core
} // namespace quickevent
