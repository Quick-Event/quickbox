#pragma once

#include "quickeventcoreglobal.h"

#include <qf/core/utils.h>

#include <QChar>

namespace quickevent {
namespace core {
	
class QUICKEVENTCORE_DECL_EXPORT CodeDef : public QVariantMap
{
public:
	static const QString CONTROL_TYPE_START;
	static const QString CONTROL_TYPE_FINISH;

	//QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(QString, t, setT, ype)
	QF_VARIANTMAP_FIELD(double, l, setL, atitude)
	QF_VARIANTMAP_FIELD(double, l, setL, ongitude)
	QF_VARIANTMAP_FIELD(int, d, setD, istance)

public:
	static constexpr int PUNCH_CODE_MIN = 10;
	static constexpr int PUNCH_CODE_MAX = 899;
	static const int START_PUNCH_CODE;
	static const int FINISH_PUNCH_CODE;
public:
	CodeDef() : QVariantMap() {}
	CodeDef(const QVariantMap &m);

	int code() const;
	void setCode(int c);

	QString toString() const;
};

}}

Q_DECLARE_METATYPE(quickevent::core::CodeDef)
