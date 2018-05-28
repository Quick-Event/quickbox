#include "siid.h"

#include <qf/core/log.h>

namespace quickevent {
namespace si {

void SiId::registerQVariantFunctions()
{
	static bool registered = false;
	if(!registered) {
		registered = true;
		qfInfo() << __FUNCTION__;
		{
			bool ok = QMetaType::registerComparators<quickevent::si::SiId>();
			if(!ok)
				qfError() << "Error registering comparators for quickevent::si::SiId!";
		}
		{
			bool ok = QMetaType::registerConverter<quickevent::si::SiId, int>([](const quickevent::si::SiId &i) -> int {return (int)i;});
			if(!ok)
				qfError() << "Error registering converter for quickevent::si::SiId!";
		}
		{
			bool ok = QMetaType::registerConverter<quickevent::si::SiId, QString>([](const quickevent::si::SiId &i) -> QString {return QString::number((int)i);});
			if(!ok)
				qfError() << "Error registering converter for quickevent::si::SiId!";
		}
	}
}

} // namespace og
} // namespace quickevent

