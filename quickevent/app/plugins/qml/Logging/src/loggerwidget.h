#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include "Logging/loggingplugin.h"

#include <qf/qmlwidgets/framework/logwidget.h>

namespace qf { namespace core { class LogEntryMap; }}

namespace qf {
namespace core {
class SignalLogDevice;
}
}

class LoggerLogDevice;

class LoggerWidget : public qf::qmlwidgets::framework::LogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::LogWidget Super;
public:
	explicit LoggerWidget(QWidget *parent = 0);
	~LoggerWidget() Q_DECL_OVERRIDE;
private:
	Logging::LoggingPlugin* loggingPlugin();

	void registerLogCategories() Q_DECL_OVERRIDE;

	//Q_SLOT void onLogEntry(const qf::core::LogEntryMap &log_entry);
private:
	LoggerLogDevice *m_logDevice = nullptr;
};

#endif // LOGGERWIDGET_H
