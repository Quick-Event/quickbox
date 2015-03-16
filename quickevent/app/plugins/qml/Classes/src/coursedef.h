#ifndef COURSEDEF_H
#define COURSEDEF_H

#include <qf/core/utils.h>

struct CourseDef : public QVariantMap
{
	//QF_VARIANTMAP_FIELD2(int, c, setC, ourseId, 0)
	QF_VARIANTMAP_FIELD(QString, c, setC, ourse)
	QF_VARIANTMAP_FIELD2(int, l, setL, enght, 0)
	QF_VARIANTMAP_FIELD2(int, c, setC, limb, 0)
	QF_VARIANTMAP_FIELD(QStringList, c, setC, lasses)
	QF_VARIANTMAP_FIELD(QVariantList, c, setC, odes)

	CourseDef() : QVariantMap() {}
	CourseDef(const QVariantMap &m) : QVariantMap(m) {}
	QString toString() const;
};

#endif // COURSEDEF_H
