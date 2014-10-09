#ifndef QF_QMLWIDGETS_REPORTS_STYLE_PEN_H
#define QF_QMLWIDGETS_REPORTS_STYLE_PEN_H

#include "istyled.h"
#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QPen>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Pen : public QObject, public IStyled
{
	Q_OBJECT
	Q_ENUMS(PenStyle)
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QVariant basedOn READ basedOn WRITE setBasedOn)
	Q_CLASSINFO("DefaultProperty", "basedOn")
	//Q_PROPERTY(qf::qmlwidgets::reports::style::Pen* basedOn READ basedOn WRITE setBasedOn)
	//Q_PROPERTY(QString basedOnName READ basedOnName WRITE setBasedOnName NOTIFY basedOnNameChanged)
	Q_PROPERTY(QVariant color READ color WRITE setColor)
	Q_PROPERTY(PenStyle style READ style WRITE setStyle)
	Q_CLASSINFO("property.width.doc", "Pen width in points (1/72 inch).\n"
				"A line width of zero indicates a cosmetic pen.")
	Q_PROPERTY(qreal width READ width WRITE setWidth)
public:
	explicit Pen(QObject *parent = 0);
	~Pen() Q_DECL_OVERRIDE;
public:
	enum PenStyle { PenUndefined  = -1,
					NoPen = Qt::NoPen,
					SolidLine = Qt::SolidLine,
					DashLine = Qt::DashLine,
					DotLine = Qt::DotLine,
					DashDotLine = Qt::DashDotLine,
					DashDotDotLine = Qt::DashDotDotLine,
					CustomDashLine = Qt::CustomDashLine };
	//QF_PROPERTY_IMPL(QString, n, N, ame)
	QF_PROPERTY_IMPL(QVariant, c, C, olor)
	QF_PROPERTY_IMPL(QVariant, b, B, asedOn)
	QF_PROPERTY_IMPL2(PenStyle, s, S, tyle, SolidLine)
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
