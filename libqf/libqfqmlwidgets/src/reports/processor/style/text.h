#ifndef QF_QMLWIDGETS_REPORTS_STYLE_TEXT_H
#define QF_QMLWIDGETS_REPORTS_STYLE_TEXT_H

#include "pen.h"
#include "brush.h"
#include "font.h"
#include "compiledtextstyle.h"

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Text : public QObject, public IStyled
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QVariant basedOn READ basedOn WRITE setBasedOn)
	Q_CLASSINFO("DefaultProperty", "basedOn")
	Q_PROPERTY(qf::qmlwidgets::reports::style::Pen* pen READ pen WRITE setPen NOTIFY penChanged)
	Q_PROPERTY(qf::qmlwidgets::reports::style::Brush* brush READ brush WRITE setBrush NOTIFY brushChanged)
	Q_PROPERTY(qf::qmlwidgets::reports::style::Font* font READ font WRITE setFont NOTIFY fontChanged)
public:
	explicit Text(QObject *parent = 0);
	~Text() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_IMPL(QVariant, b, B, asedOn)
	QF_PROPERTY_OBJECT_IMPL(Pen*, p, P, en)
	QF_PROPERTY_OBJECT_IMPL(Brush*, b, B, rush)
	QF_PROPERTY_OBJECT_IMPL(Font*, f, F, ont)
public:
	CompiledTextStyle textStyle();
private:
	CompiledTextStyle m_textStyle;
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_TEXT_H
