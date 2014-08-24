
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "reportprocessorcontext.h"

#include <qf/core/log.h>

namespace qfg = qf::qmlwidgets::graphics;

using namespace qf::qmlwidgets::reports;

//=================================================
//             ReportProcessorContext
//=================================================
const ReportProcessorContext & ReportProcessorContext::sharedNull()
{
	static ReportProcessorContext n = ReportProcessorContext(SharedDummyHelper());
	return n;
}

ReportProcessorContext::ReportProcessorContext(ReportProcessorContext::SharedDummyHelper )
{
	d = new Data();
}

ReportProcessorContext::ReportProcessorContext()
{
	*this = sharedNull();
}

ReportProcessorContext::ReportProcessorContext(const qfg::StyleCache &style_cache)
{
	d = new Data();
	d->styleCache = style_cache;
}

void ReportProcessorContext::clear()
{
	d->options.clear();
	d->styleCache.clearCache();
}
