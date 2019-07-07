#pragma once

#include "../quickeventcoreglobal.h"

#include <qf/core/utils.h>

#include <QChar>

namespace quickevent {
namespace core {
namespace si {
	
class QUICKEVENTCORE_DECL_EXPORT CodeDef : public QVariantMap
{
public:
	static const QString CONTROL_TYPE_START;
	static const QString CONTROL_TYPE_FINISH;

	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(QString, t, setT, ype)
	QF_VARIANTMAP_FIELD(double, l, setL, atitude)
	QF_VARIANTMAP_FIELD(double, l, setL, ongitude)
	QF_VARIANTMAP_FIELD(int, d, setD, istance)

public:
	CodeDef() : QVariantMap() {}
	CodeDef(const QVariantMap &m) : QVariantMap(m) {}

	QString toString() const;
};

}}}
