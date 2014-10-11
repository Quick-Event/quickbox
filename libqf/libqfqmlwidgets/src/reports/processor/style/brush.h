#ifndef QF_QMLWIDGETS_REPORTS_STYLE_BRUSH_H
#define QF_QMLWIDGETS_REPORTS_STYLE_BRUSH_H

#include "styleobject.h"
#include "color.h"

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QBrush>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Brush : public StyleObject
{
	Q_OBJECT
	Q_ENUMS(BrushStyle)
	Q_PROPERTY(QVariant basedOn READ basedOn WRITE setBasedOn)
	Q_CLASSINFO("DefaultProperty", "basedOn")
	Q_PROPERTY(qf::qmlwidgets::reports::style::Color* color READ color WRITE setColor NOTIFY colorChanged)
	Q_PROPERTY(BrushStyle style READ style WRITE setStyle)
public:
	explicit Brush(QObject *parent = 0);
	~Brush() Q_DECL_OVERRIDE;
public:
	enum BrushStyle {
		NoBrush = Qt::NoBrush,
		SolidPattern = Qt::SolidPattern,
		HorPattern = Qt::HorPattern,
		DotLine = Qt::VerPattern,
		VerPattern = Qt::CrossPattern,
		BDiagPattern = Qt::BDiagPattern,
		FDiagPattern = Qt::FDiagPattern,
		DiagCrossPattern = Qt::DiagCrossPattern
	};
	QF_PROPERTY_OBJECT_IMPL(Color*, c, C, olor)
	QF_PROPERTY_IMPL(QVariant, b, B, asedOn)
	QF_PROPERTY_IMPL2(BrushStyle, s, S, tyle, NoBrush)
public:
	QBrush brush();
private:
	QBrush m_brush;
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_BRUSH_H
