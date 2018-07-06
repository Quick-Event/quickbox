#include "siid.h"

#include <qf/core/log.h>

namespace quickevent {
namespace core {
namespace si {

void SiId::registerQVariantFunctions()
{
	static bool registered = false;
	if(!registered) {
		registered = true;
		qfInfo() << __FUNCTION__;
		{
			bool ok = QMetaType::registerComparators<SiId>();
			if(!ok)
				qfError() << "Error registering comparators for quickevent::core::si::SiId!";
		}
		{
			bool ok = QMetaType::registerConverter<SiId, int>([](const SiId &i) -> int {return (int)i;});
			if(!ok)
				qfError() << "Error registering converter for quickevent::core::si::SiId!";
		}
		{
			bool ok = QMetaType::registerConverter<SiId, QString>([](const SiId &i) -> QString {return QString::number((int)i);});
			if(!ok)
				qfError() << "Error registering converter for quickevent::core::si::SiId!";
		}
	}
}

}}}
