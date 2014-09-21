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
private:
	typedef QQmlComponent Super;
public:
	explicit ReportDocument(QQmlEngine *engine, const QString &file_name, QObject *parent = 0);
public:
	QString fileName() const {return m_fileName;}
	void setFileName(const QString &fname) {m_fileName = fname;}
protected:
	QString m_fileName;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H
