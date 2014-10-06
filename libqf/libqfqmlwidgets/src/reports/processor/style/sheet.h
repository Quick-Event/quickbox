#ifndef QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H
#define QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H

#include "color.h"
#include "pen.h"

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QString>
#include <QQmlListProperty>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Sheet : public QObject
{
	Q_OBJECT
	/// set name of prototype or prototype itself here
	Q_PROPERTY(QVariant basedOn READ basedOn WRITE setBasedOn)
	Q_PROPERTY(QQmlListProperty<qf::qmlwidgets::reports::style::Color> colors READ colors)
	Q_PROPERTY(QQmlListProperty<qf::qmlwidgets::reports::style::Pen> pens READ pens)
public:
	explicit Sheet(QObject *parent = nullptr);
	~Sheet() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_IMPL(QVariant, b, B, asedOn)
public:
	Pen* penForName(const QString &name);
	void setPenForName(const QString &name, Pen *p);
private:
	QQmlListProperty<Color> colors();
	QQmlListProperty<Pen> pens();
private:
	QList<Color*> m_colors;
	QList<Pen*> m_pens;
	QMap<QString, Pen*> m_definedPens;
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H
