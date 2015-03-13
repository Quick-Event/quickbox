#ifndef QF_CORE_UTILS_TIMESCOPE_H
#define QF_CORE_UTILS_TIMESCOPE_H

#include "../core/coreglobal.h"

#include <QElapsedTimer>
#include <QString>

//#define QF_TIMESCOPE_ENABLED

#ifdef QF_TIMESCOPE_ENABLED
#define QF_TIME_SCOPE(name) qf::core::utils::TimeScope __qf_time_scope (name)
#else
#define QF_TIME_SCOPE(name)
#endif

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT TimeScope
{
public:
	TimeScope(const QString &name = QString());
	~TimeScope();
private:
	int m_id;
	QString m_name;
	QElapsedTimer m_startTime;
	static int s_id;
	static int s_level;
};

} // namespace utils
} // namespace core
} // namespace qf

#endif // QF_CORE_UTILS_TIMESCOPE_H
