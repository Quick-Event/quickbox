#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include "Logging/loggingplugin.h"

#include <QFrame>

namespace qf { namespace core { class LogEntryMap; }}

namespace Ui {
class LoggerWidget;
}

namespace qf {
namespace core {
class SignalLogDevice;
}
}

class LoggerWidget : public QFrame
{
	Q_OBJECT
public:
	explicit LoggerWidget(QWidget *parent = 0);
	~LoggerWidget() Q_DECL_OVERRIDE;
private:
	Logging::LoggingPlugin* loggingPlugin();

	Q_SLOT void onLogEntry(const qf::core::LogEntryMap &log_entry);
	Q_SLOT void onLogLevelSet(int ix);
private:
	Ui::LoggerWidget *ui;
	qf::core::SignalLogDevice *m_logDevice;
};

#endif // LOGGERWIDGET_H
