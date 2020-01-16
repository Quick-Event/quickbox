#ifndef QUICKEVENT_CORE_COURSEDEF_H
#define QUICKEVENT_CORE_COURSEDEF_H

#include "quickeventcoreglobal.h"
#include "codedef.h"

#include <qf/core/utils.h>

namespace quickevent {
namespace core {

class QUICKEVENTCORE_DECL_EXPORT CourseDef : public QVariantMap
{
	QF_VARIANTMAP_FIELD(int, i, setI, d)
	QF_VARIANTMAP_FIELD(QString, n, setN, ame)
	QF_VARIANTMAP_FIELD(int, l, setL, enght)
	QF_VARIANTMAP_FIELD(int, c, setC, limb)
	QF_VARIANTMAP_FIELD(CodeDef, f, setF, inishCode)
	QF_VARIANTMAP_FIELD(CodeDef, s, setS, tartCode)
	QF_VARIANTMAP_FIELD(QVariantList, c, setC, odes)

public:
	CourseDef() : QVariantMap() {}
	CourseDef(const QVariantMap &m) : QVariantMap(m) {}
};

} // namespace core
} // namespace quickevent

#endif // QUICKEVENT_CORE_COURSEDEF_H
