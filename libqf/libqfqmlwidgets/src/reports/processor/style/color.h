#ifndef QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H
#define QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QColor>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Color : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QVariant definition READ definition WRITE setDefinition NOTIFY definitionChanged)
public:
	explicit Color(QObject *parent = nullptr);
	~Color() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_IMPL(QVariant, d, D, efinition)

	QString name() {return objectName();}
	void setName(const QString &s) {setObjectName(s);}

	QColor color();
private:
	static QString nextSequentialName();
	Q_SLOT void setDirty() {m_dirty = true;}
private:
	bool m_dirty;
	QColor m_color;
};

}}}}

#endif
