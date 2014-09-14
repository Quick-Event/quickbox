#ifndef QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H
#define QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H

#include <qf/core/exception.h>

#include <QQmlComponent>
#include <QCoreApplication>

namespace qf {
namespace qmlwidgets {
namespace reports {

class ReportDocument : public QQmlComponent
{
public:
	explicit ReportDocument();
	explicit ReportDocument(const QString &root_name);
public:
	QString fileName() const {return m_fileName;}
	void setFileName(const QString &fname) {m_fileName = fname;}
protected:
	QString m_fileName;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H
