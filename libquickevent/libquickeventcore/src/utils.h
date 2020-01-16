#ifndef QUICKEVENT_CORE_UTILS_H
#define QUICKEVENT_CORE_UTILS_H

#include "quickeventcoreglobal.h"

class QDateTime;

namespace quickevent {
namespace core {

class QUICKEVENTCORE_DECL_EXPORT Utils
{
public:
	static QString dateTimeToIsoStringWithUtcOffset(const QDateTime &dt);
};

} // namespace core
} // namespace quickevent

#endif // QUICKEVENT_CORE_UTILS_H
