#pragma once

#include "quickeventcoreglobal.h"

#include <qf/core/utils.h>

#include <QChar>

#include <optional>

namespace quickevent {
namespace core {
	
class QUICKEVENTCORE_DECL_EXPORT CodeDef : public QVariantMap
{
public:
	enum class Type {Invalid, Start, Finish, Control};
	static const char* typeToString(Type t);
	//static const QString CONTROL_TYPE_START;
	//static const QString CONTROL_TYPE_FINISH;

	//QF_VARIANTMAP_FIELD(int, c, setC, ode)
	//QF_VARIANTMAP_FIELD(QString, t, setT, ype)
	QF_VARIANTMAP_FIELD(double, l, setL, atitude)
	QF_VARIANTMAP_FIELD(double, l, setL, ongitude)
	QF_VARIANTMAP_FIELD(int, d, setD, istance)

public:
	static const int PUNCH_CODE_MIN;
	static const int PUNCH_CODE_MAX;
	static const int START_PUNCH_CODE;
	static const int FINISH_PUNCH_CODE;
public:
	CodeDef() : QVariantMap() {}
	CodeDef(const QVariantMap &m);
	CodeDef(const QString &code_str) { setCode(codeFromString(code_str)); }

	Type type() const;
	int code() const;
	void setCode(int c);
	void setCode(const QString &code_str) { setCode(codeFromString(code_str)); }

	static int codeFromString(const QString &code_str);
	static QString codeToString(int code);
	static Type codeToType(int code);
	static std::optional<int> codeToStartNumber(int code);
	static std::optional<int> codeToFinishNumber(int code);

	QString toString() const;
};

}}

Q_DECLARE_METATYPE(quickevent::core::CodeDef)
