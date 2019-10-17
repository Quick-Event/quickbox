#ifndef QF_QMLWIDGETS_REPORTS_STYLE_FONT_H
#define QF_QMLWIDGETS_REPORTS_STYLE_FONT_H

#include "styleobject.h"

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QFont>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Font : public StyleObject
{
	Q_OBJECT
	Q_ENUMS(FontStyle)
	Q_ENUMS(FontStyleHint)
	Q_ENUMS(FontWeight)
	// there are some restrictions for assigning primitive types to default properties, see: pen.h
	Q_CLASSINFO("DefaultProperty", "basedOn")
	Q_PROPERTY(QVariant basedOn READ basedOn WRITE setBasedOn)
	Q_PROPERTY(QString family READ family WRITE setFamily NOTIFY familyChanged)
	Q_PROPERTY(FontStyle style READ style WRITE setStyle NOTIFY styleChanged)
	Q_PROPERTY(FontStyleHint hint READ hint WRITE setHint NOTIFY hintChanged)
	Q_PROPERTY(FontWeight weight READ weight WRITE setWeight NOTIFY weightChanged)
	Q_PROPERTY(qreal pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
public:
	explicit Font(QObject *parent = nullptr);
	~Font() Q_DECL_OVERRIDE;
public:
	enum FontStyle {
		StyleNormal = QFont::StyleNormal,
		StyleItalic = QFont::StyleItalic,
		StyleOblique = QFont::StyleOblique,
		StyleInherited
	};
	enum FontStyleHint {
		HintAnyStyle = QFont::AnyStyle,
		HintSansSerif = QFont::SansSerif,
		HintHelvetica = QFont::Helvetica,
		HintSerif = QFont::Serif,
		HintTimes = QFont::Times,
		HintTypeWriter = QFont::TypeWriter,
		HintCourier = QFont::Courier,
		HintOldEnglish = QFont::OldEnglish,
		HintDecorative = QFont::Decorative,
		HintMonospace = QFont::Monospace,
		HintFantasy = QFont::Fantasy,
		HintCursive = QFont::Cursive,
		HintSystem = QFont::System,
		HintInherited
	};
	enum FontWeight {
		WeightLight = QFont::Light,
		WeightNormal = QFont::Normal,
		WeightDemiBold = QFont::DemiBold,
		WeightBold = QFont::Bold,
		WeightBlack = QFont::Black,
		WeightInherited
	};
	QF_PROPERTY_IMPL(QVariant, b, B, asedOn)
	QF_PROPERTY_IMPL(QString, f, F, amily)
	QF_PROPERTY_IMPL2(FontStyle, s, S, tyle, StyleInherited)
	QF_PROPERTY_IMPL2(FontStyleHint, h, H, int, HintInherited)
	QF_PROPERTY_IMPL2(FontWeight, w, W, eight, WeightInherited)
	QF_PROPERTY_IMPL2(qreal, p, P, ointSize, 0)
public:
	QFont font();
private:
	QFont m_font;
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_FONT_H
