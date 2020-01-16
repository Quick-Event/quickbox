#include "checkedpunch.h"
#include "../codedef.h"

namespace quickevent {
namespace core {
namespace si {

CheckedPunch CheckedPunch::fromCodeDef(const quickevent::core::CodeDef &cd)
{
	CheckedPunch ret;
	ret.setCode(cd.code());
	ret.setDistance(cd.distance());
	return ret;
}

}}}
