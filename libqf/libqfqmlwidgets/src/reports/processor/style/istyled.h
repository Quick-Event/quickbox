#ifndef QF_QMLWIDGETS_REPORTS_STYLE_STYLEDBASE_H
#define QF_QMLWIDGETS_REPORTS_STYLE_STYLEDBASE_H

class QObject;

namespace qf {
namespace qmlwidgets {
namespace reports {
class ReportItemReport;

class ReportProcessor;

namespace style {

class Sheet;

class IStyled
{
public:
	IStyled(QObject *style_object);
protected:
	ReportItemReport* reportItem();
	ReportProcessor* reportProcessor();
	Sheet* reportStyleSheet();
private:
	QObject *m_styleObject;
};

}}}}

#endif
