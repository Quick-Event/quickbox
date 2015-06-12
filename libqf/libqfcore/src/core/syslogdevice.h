#ifndef QF_CORE_SYSLOGDEVICE_H
#define QF_CORE_SYSLOGDEVICE_H

#include "logdevice.h"

#include <QObject>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT SysLogDevice : public LogDevice
{
	Q_OBJECT
private:
	typedef LogDevice Super;
protected:
	explicit SysLogDevice(QObject *parent = 0);
public:
	~SysLogDevice() Q_DECL_OVERRIDE;
public:
	static SysLogDevice* install();

	void log(Log::Level level, const QMessageLogContext &context, const QString &msg) Q_DECL_OVERRIDE;
};

} // namespace core
} // namespace qf

#endif // QF_CORE_SYSLOGDEVICE_H
