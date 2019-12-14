#ifndef TABLEMODELLOGDEVICE_H
#define TABLEMODELLOGDEVICE_H

#include <qf/core/logdevice.h>

namespace qf { namespace core { namespace model { class LogTableModel; } } }

#define TABLE_MODEL_LOG_DEVICE "table_model_log_device"

class TableModelLogDevice : public qf::core::SignalLogDevice
{
	Q_OBJECT
private:
	typedef qf::core::SignalLogDevice Super;
protected:
	TableModelLogDevice(QObject *parent = nullptr);
public:
	static TableModelLogDevice* install()
	{
		auto *ret = new TableModelLogDevice();
		LogDevice::install(ret);
		return ret;
	}

	qf::core::model::LogTableModel* logTableModel();

	void setCategories(const QMap<QString, qf::core::Log::Level> &cats);
public:
	using Super::isMatchingLogFilter;
	bool isMatchingLogFilter(qf::core::Log::Level level, const char *file_name, const char *category) Q_DECL_OVERRIDE;
private:
	LogFilter m_logFilter;
	qf::core::model::LogTableModel *m_logTableModel = nullptr;
};

#endif // TABLEMODELLOGDEVICE_H
