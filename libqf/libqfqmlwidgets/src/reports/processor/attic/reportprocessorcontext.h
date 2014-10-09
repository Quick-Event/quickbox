
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTPROCESSORCONTEXT_H
#define QF_QMLWIDGETS_REPORTS_REPORTPROCESSORCONTEXT_H

#include "../../qmlwidgetsglobal.h"
#include "../../graphics/stylecache.h"

#include <QSharedData>
#include <QVariantMap>

namespace qf {
namespace qmlwidgets {
namespace reports {

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportProcessorContext
{
private:
	struct Data : public QSharedData
	{
		qf::qmlwidgets::graphics::StyleCache styleCache;
		//QVariantMap options;
		//KeyVals keyVals;
	};
	QSharedDataPointer<Data> d;
	class SharedDummyHelper {};
	ReportProcessorContext(SharedDummyHelper);
	static const ReportProcessorContext& sharedNull();
public:
	bool isNull() const {return d == sharedNull().d;}
	const qf::qmlwidgets::graphics::StyleCache& styleCache() const {return d->styleCache;}
	qf::qmlwidgets::graphics::StyleCache& styleCacheRef() {return d->styleCache;}
	void clear();
public:
	ReportProcessorContext();
	/// protoze je qf::qmlwidgets::graphics::StyleCache explicitne sdilena, potrebuju konstruktor s jinou cachi, nez ma null() context, protoze jinak se styleCache chova jako staticka promenna
	/// vsechny contexty pouzivaji stejnou cache a to tu, kterou zalozi null context
	//ReportProcessorContext(const StyleCache &style_cache);
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPRTPROCESSORCONTEXT_H

