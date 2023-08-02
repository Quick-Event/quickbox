#ifndef QF_QMLWIDGETS_STYLE_H
#define QF_QMLWIDGETS_STYLE_H

#include "qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QSize>

class QFileInfo;

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Style : public QObject
{
	Q_OBJECT
public:
	explicit Style(QObject *parent = nullptr);

	void addIconSearchPath(const QString &p);

	const QSize& defaultIconSize() const {return m_defaultIconSize;}

	QPixmap pixmap(const QString &name, const QSize &pixmap_size = QSize());
	QPixmap pixmap(const QString &name, int height);
	QIcon icon(const QString &name, const QSize &pixmap_size = QSize());

	static Style* instance();
	static void setInstance(Style *style);
private:
	QPixmap pixmapFromSvg(const QString &file_name, const QSize &pixmap_size = QSize());
	QFileInfo findFile(const QString &path, const QString &default_extension) const;
private:
	QSize m_defaultIconSize;
	QStringList m_iconSearchPaths;
};

}}

#endif // QF_QMLWIDGETS_STYLE_H
