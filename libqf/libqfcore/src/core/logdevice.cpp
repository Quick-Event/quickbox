#include <qf/core/logdevice.h>
#include <qf/core/log.h>

using namespace qf::core;

LogDevice::LogDevice()
 : m_logTreshold(Log::LOG_ERR), m_stackLevel(0)
{
}
