#ifndef QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H
#define QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H

#include "istyled.h"

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QColor>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Color : public QObject, public IStyled
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
    Q_CLASSINFO("DefaultProperty", "definition")
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
    //Q_SLOT void setDirty();
private:
	QColor m_color;
};

}}}}

#endif
