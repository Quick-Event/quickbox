#include "timescope.h"
#include "../core/log.h"

namespace qf {
namespace core {
namespace utils {

int TimeScope::s_id = 0;
int TimeScope::s_level = 0;

TimeScope::TimeScope(const QString &name)
{
	m_name = name;
	m_id = ++TimeScope::s_id;
	TimeScope::s_level++;
	m_startTime.start();
	qfInfo() << m_id << '[' << s_level << ']' << m_name << "<-----";
}

TimeScope::~TimeScope()
{
	qint64 nsec = m_startTime.nsecsElapsed();
	qfDebug() << m_id << '[' << TimeScope::s_level << ']' << m_name << "----->" << (nsec/1000000) << "msec," << nsec << "nsec";
	TimeScope::s_level--;
}

} // namespace utils
} // namespace core
} // namespace qf

