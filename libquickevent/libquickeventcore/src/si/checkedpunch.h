#pragma once

#include "../quickeventcoreglobal.h"

#include <qf/core/utils.h>

#include <QVariantMap>

namespace quickevent {
namespace core {

class CodeDef;

namespace si {


class QUICKEVENTCORE_DECL_EXPORT CheckedPunch : public QVariantMap
{
	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	QF_VARIANTMAP_FIELD(int, s, setS, tpTimeMs)
	QF_VARIANTMAP_FIELD(int, l, setL, apTimeMs)
	QF_VARIANTMAP_FIELD(int, d, setD, istance)
public:
	CheckedPunch(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}

	static CheckedPunch fromCodeDef(const quickevent::core::CodeDef &cd);
};

}}}

