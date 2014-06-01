#ifndef QF_CORE_STACKTRACE_H
#define QF_CORE_STACKTRACE_H

#include "coreglobal.h"

class QString;

namespace qf {
namespace core {

/// Helper class generating the current stack trace
/**
 Currently only Linux is supported.:((
*/
class QFCORE_DECL_EXPORT StackTrace
{
public:
	static QString stackTrace();
protected:
	static QString trace2str();
};

}
}

#endif // QF_CORE_STACKTRACE_H
