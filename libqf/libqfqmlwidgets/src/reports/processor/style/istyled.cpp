#include "istyled.h"
#include "../reportitem.h"

#include <qf/core/log.h>

#include <QObject>

using namespace qf::qmlwidgets::reports::style;

IStyled::IStyled(QObject *style_object)
	: m_styleObject(style_object)
{
}

qf::qmlwidgets::reports::ReportItemReport *IStyled::reportItem()
{
	ReportItemReport *ret = nullptr;
	QObject *it = m_styleObject;
	while(it) {
		ret = qobject_cast<ReportItemReport*>(it);
		if(ret) {
			break;
		}
		it = it->QObject::parent();
	}
	QF_ASSERT(ret != nullptr, "Cannot find Report Root Item", return ret);
	return ret;
}

qf::qmlwidgets::reports::ReportProcessor *IStyled::reportProcessor()
{
	ReportProcessor *ret = nullptr;
	ReportItemReport *rir = reportItem();
	if(rir) {
		ret = rir->reportProcessor();
	}
	QF_ASSERT(ret != nullptr, "Report Style Item without ReportProcessor", return ret);
	return ret;
}

Sheet *IStyled::reportStyleSheet()
{
	Sheet *ret = nullptr;
	ReportItemReport *rir = reportItem();
	if(rir)
		ret = rir->styleSheet();
	return ret;
}
