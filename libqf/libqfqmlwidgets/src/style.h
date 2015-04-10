#ifndef QF_QMLWIDGETS_STYLE_H
#define QF_QMLWIDGETS_STYLE_H

#include "qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QSize>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Style : public QObject
{
	Q_OBJECT
public:
	explicit Style(QObject *parent = 0);

	QF_PROPERTY_IMPL2(QString, i, I, conPath, QStringLiteral(":/qf/qmlwidgets/images/"))
	QF_PROPERTY_IMPL2(QSize, d, D, efaultIconSize, QSize(22, 22))

	QPixmap pixmapFromSvg(const QString &name, const QSize &pixmap_size);
	QIcon icon(const QString &name);

	static Style* instance();
	static void setInstance(Style *style);
private:
};

}}

#endif // QF_QMLWIDGETS_STYLE_H
