#pragma once

#include "../quickeventcoreglobal.h"

#include <qf/core/utils.h>

#include <QVariantMap>

namespace quickevent {
namespace core {
namespace si {

class QUICKEVENTCORE_DECL_EXPORT CheckedPunch : public QVariantMap
{
	QF_VARIANTMAP_FIELD(int, c, setC, ode)
	//QF_VARIANTMAP_FIELD2(int, p, setP, osition, 0)
	QF_VARIANTMAP_FIELD(int, s, setS, tpTimeMs)
	QF_VARIANTMAP_FIELD(int, l, setL, apTimeMs)
public:
	CheckedPunch(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
};

}}}

