#ifndef QF_CORE_STACKTRACE_H
#define QF_CORE_STACKTRACE_H

#include "coreglobal.h"

#include <QStringList>

namespace qf {
namespace core {

/// Helper class generating the current stack trace
/**
 Currently only Linux is supported.:((
*/
class QFCORE_DECL_EXPORT StackTrace
{
public:
	static StackTrace stackTrace();
	QString toString() const;
	QStringList trace() const;
protected:
	static QStringList trace2str();
private:
	StackTrace(const QStringList &sl);
	QStringList m_trace;
};

}
}

#endif // QF_CORE_STACKTRACE_H
