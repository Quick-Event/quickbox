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
	//QF_PROPERTY_IMPL(QSize, d, D, efaultIconSize)

	const QSize& defaultIconSize() const {return m_defaultIconSize;}

	QPixmap pixmapFromSvg(const QString &name, const QSize &pixmap_size = QSize());
	QPixmap pixmap(const QString &name, const QSize &pixmap_size = QSize());
	QPixmap pixmap(const QString &name, int height);
	QIcon icon(const QString &name, const QSize &pixmap_size = QSize());

	static Style* instance();
	static void setInstance(Style *style);
private:
	QSize m_defaultIconSize;
};

}}

#endif // QF_QMLWIDGETS_STYLE_H
