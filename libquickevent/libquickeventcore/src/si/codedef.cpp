#include "codedef.h"

namespace quickevent {
namespace core {
namespace si {
	
const QString CodeDef::CONTROL_TYPE_START = "S";
const QString CodeDef::CONTROL_TYPE_FINISH = "F";
const QString CodeDef::CONTROL_TYPE_CONTROL = "C";

QString CodeDef::toString() const
{
	QString ret;
	ret += "code: " + QString::number(code()) + " type: " + QString(type());
	ret += " latitude: " + QString("%1").arg(latitude(), 0, 'f', 8);
	ret += " longitude: " + QString("%1").arg(longitude(), 0, 'f', 8);
	return ret;
}

}}}
