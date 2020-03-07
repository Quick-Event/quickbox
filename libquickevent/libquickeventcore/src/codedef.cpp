#include "codedef.h"

namespace quickevent {
namespace core {
	
const int CodeDef::START_PUNCH_CODE = 10;
const int CodeDef::FINISH_PUNCH_CODE = 900;

const int CodeDef::PUNCH_CODE_MIN = 30;
const int CodeDef::PUNCH_CODE_MAX = CodeDef::FINISH_PUNCH_CODE - 1;

//const QString CodeDef::CONTROL_TYPE_START = "S";
//const QString CodeDef::CONTROL_TYPE_FINISH = "F";

const char *CodeDef::typeToString(CodeDef::Type t)
{
	switch (t) {
	case Type::Start: return "Start";
	case Type::Finish: return "Finish";
	case Type::Control: return "Control";
	case Type::Invalid: break;
	}
	return "Invalid";
}

CodeDef::CodeDef(const QVariantMap &m)
	: QVariantMap(m)
{
	//if(type() == CONTROL_TYPE_FINISH)
	//	setCode(FINISH_PUNCH_CODE);
}

CodeDef::Type CodeDef::type() const
{
	return codeToType(code());
}

int CodeDef::code() const
{
	return this->value(QStringLiteral("code")).toInt();
}

void CodeDef::setCode(int c)
{
	(*this)[QStringLiteral("code")] = c;
}

int CodeDef::codeFromString(const QString &code_str)
{
	if(code_str.startsWith('S')) {
		bool ok;
		int code = code_str.mid(1).toInt(&ok);
		if(!ok || code < 1)
			QF_EXCEPTION(QString("Invalid start code '%1'").arg(code_str));
		return (START_PUNCH_CODE + code - 1);
	}
	else if(code_str.startsWith('F')) {
		bool ok;
		int code = code_str.mid(1).toInt(&ok);
		if(!ok || code < 1)
			QF_EXCEPTION(QString("Invalid finish code '%1'").arg(code_str));
		return (FINISH_PUNCH_CODE + code - 1);
	}
	else {
		bool ok;
		int code = code_str.toInt(&ok);
		if(!ok || code < 1)
			QF_EXCEPTION(QString("Invalid control code '%1'").arg(code_str));
		return code;
	}
}

QString CodeDef::codeToString(int code)
{
	if(code >= START_PUNCH_CODE && code < PUNCH_CODE_MIN)
		return 'S' + QString::number(code - START_PUNCH_CODE + 1);
	if(code >= FINISH_PUNCH_CODE)
		return 'F' + QString::number(code - FINISH_PUNCH_CODE + 1);
	return QString::number(code);
}

CodeDef::Type CodeDef::codeToType(int code)
{
	if(code >= FINISH_PUNCH_CODE)
		return Type::Finish;
	if(code >= START_PUNCH_CODE && code < PUNCH_CODE_MIN)
		return Type::Start;
	if(code >= PUNCH_CODE_MIN && code <= PUNCH_CODE_MAX)
		return Type::Control;
	return Type::Invalid;
}

QString CodeDef::toString() const
{
	QString ret;
	ret += "code: " + QString::number(code()) + " type: " + typeToString(type());
	ret += " latitude: " + QString("%1").arg(latitude(), 0, 'f', 8);
	ret += " longitude: " + QString("%1").arg(longitude(), 0, 'f', 8);
	return ret;
}

}}
