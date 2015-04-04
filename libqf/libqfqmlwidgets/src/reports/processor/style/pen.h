#ifndef QF_QMLWIDGETS_REPORTS_STYLE_PEN_H
#define QF_QMLWIDGETS_REPORTS_STYLE_PEN_H

#include "styleobject.h"
#include "color.h"
#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QPen>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Pen : public StyleObject
{
	Q_OBJECT
	Q_ENUMS(PenStyle)
	// From qt doc:
	// A default property is the property to which a value is assigned
	// if an object is declared within another object's definition without declaring it
	// as a value for a particular property.
	//
	//deault property seems to work just for objects not for primitive types like string
	// border: Pen { "black1" } WILL NOT work
	// border: Pen { basedOn: "black1" } WILL work
	// border: Pen { Pen { ... } } WILL work
	Q_CLASSINFO("DefaultProperty", "basedOn")
	Q_PROPERTY(QVariant basedOn READ basedOn WRITE setBasedOn)
	Q_PROPERTY(qf::qmlwidgets::reports::style::Color* color READ color WRITE setColor NOTIFY colorChanged)
	Q_PROPERTY(PenStyle style READ style WRITE setStyle NOTIFY styleChanged)
	Q_CLASSINFO("property.width.doc", "Pen width in points (1/72 inch).\n"
				"A line width of zero indicates a cosmetic pen.")
	Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
public:
	explicit Pen(QObject *parent = 0);
	~Pen() Q_DECL_OVERRIDE;
public:
	enum PenStyle {
		NoPen = Qt::NoPen,
		SolidLine = Qt::SolidLine,
		DashLine = Qt::DashLine,
		DotLine = Qt::DotLine,
		DashDotLine = Qt::DashDotLine,
		DashDotDotLine = Qt::DashDotDotLine,
		CustomDashLine = Qt::CustomDashLine
	};
	//QF_PROPERTY_IMPL(QString, n, N, ame)
	QF_PROPERTY_OBJECT_IMPL(Color*, c, C, olor)
	QF_PROPERTY_IMPL(QVariant, b, B, asedOn)
	QF_PROPERTY_IMPL2(PenStyle, s, S, tyle, NoPen)
	QF_PROPERTY_IMPL(qreal, w, W, idth)
public:
	QPen pen();
public:
    //void setName(const QString &s) Q_DECL_OVERRIDE;
private:
    //static QString nextSequentialName();
    //Q_SLOT void setDirty() {m_dirty = true;}
    //Pen* penForName(const QString &name);
private:
	QPen m_pen;
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_PEN_H
