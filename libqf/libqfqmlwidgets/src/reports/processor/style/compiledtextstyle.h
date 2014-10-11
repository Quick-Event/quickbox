#ifndef QF_QMLWIDGETS_REPORTS_STYLE_COMPILEDTEXTSTYLE_H
#define QF_QMLWIDGETS_REPORTS_STYLE_COMPILEDTEXTSTYLE_H

#include <qf/core/utils.h>

#include <QSharedData>
#include <QFont>
#include <QPen>
#include <QBrush>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class CompiledTextStyle
{
private:
	class SharedDummyHelper {};
	struct Data : public QSharedData
	{
		QFont font;
		QPen pen;
	};
	QSharedDataPointer<Data> d;

	CompiledTextStyle(SharedDummyHelper); /// null
	static const CompiledTextStyle& sharedNull();
public:
	CompiledTextStyle();
	CompiledTextStyle(const QFont &f);

	bool isNull() const {return d == sharedNull().d;}

	QF_SHARED_CLASS_FIELD_RW(QFont, f, setF, ont)
	QF_SHARED_CLASS_FIELD_RW(QPen, p, setP, en)
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_COMPILEDTEXTSTYLE_H
