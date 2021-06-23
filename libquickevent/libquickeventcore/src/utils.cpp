#include "utils.h"

#include <QDateTime>

namespace quickevent {
namespace core {

 QString Utils::dateTimeToIsoStringWithUtcOffset(QDateTime dt)
{
	// offset is missing if not explicitly set, see https://bugreports.qt.io/browse/QTBUG-26161?focusedCommentId=554227
	dt.setOffsetFromUtc(dt.offsetFromUtc());
	return dt.toString(Qt::ISODate);
}

} // namespace core
} // namespace quickevent
