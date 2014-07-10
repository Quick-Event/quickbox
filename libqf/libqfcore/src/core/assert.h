#ifndef QFCORE_ASSERT_H
#define QFCORE_ASSERT_H

#include "log.h"

//#define QF_ASSERT_STRINGIFY_HELPER(x) #x
//#define QF_ASSERT_STRINGIFY(x) QF_ASSERT_STRINGIFY_HELPER(x)
#define QF_ASSERT_STRING(cond, message) qfError() << "\"" cond"\" -" << message << Q_FUNC_INFO

// The 'do {...} while (0)' idiom is not used for the main block here to be
// able to use 'break' and 'continue' as 'actions'.

#define QF_ASSERT(cond, message, action) if (cond) {} else { QF_ASSERT_STRING(#cond, message); action; } do {} while (0)
#define QF_ASSERT_EX(cond, message) if (cond) {} else { QF_ASSERT_STRING(#cond, message); qfFatal("Giving up!"); } do {} while (0)
#define QF_CHECK(cond, message) if (cond) {} else { QF_ASSERT_STRING(#cond, message); } do {} while (0)

#endif
