#include "codedef.h"

namespace quickevent {
namespace core {
	
const int CodeDef::START_PUNCH_CODE = 997;
const int CodeDef::FINISH_PUNCH_CODE = 999;

const QString CodeDef::CONTROL_TYPE_START = "S";
const QString CodeDef::CONTROL_TYPE_FINISH = "F";

CodeDef::CodeDef(const QVariantMap &m)
	: QVariantMap(m)
{
	//if(type() == CONTROL_TYPE_FINISH)
	//	setCode(FINISH_PUNCH_CODE);
}

int CodeDef::code() const
{
	auto t = type();
	if(t == CONTROL_TYPE_START)
		return FINISH_PUNCH_CODE;
	if(t == CONTROL_TYPE_FINISH)
		return START_PUNCH_CODE;
	return this->value(QStringLiteral("code")).toInt();
}

void CodeDef::setCode(int c)
{
	(*this)[QStringLiteral("code")] = c;
}

QString CodeDef::toString() const
{
	QString ret;
	ret += "code: " + QString::number(code()) + " type: " + QString(type());
	ret += " latitude: " + QString("%1").arg(latitude(), 0, 'f', 8);
	ret += " longitude: " + QString("%1").arg(longitude(), 0, 'f', 8);
	return ret;
}

}}
