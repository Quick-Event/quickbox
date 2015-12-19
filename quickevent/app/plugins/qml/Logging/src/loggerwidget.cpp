#include "loggerwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QComboBox>

class LoggerLogDevice : public qf::core::SignalLogDevice
{
private:
	typedef qf::core::SignalLogDevice Super;
protected:
	LoggerLogDevice(QObject *parent = 0) : Super(parent) {}
public:
	static LoggerLogDevice* install()
	{
		auto *ret = new LoggerLogDevice();
		LogDevice::install(ret);
		return ret;
	}

	//void setLogTreshold(qf::core::Log::Level log_level) {m_logFilter.defaultLogTreshold = log_level;}
	void setCategories(const QMap<QString, qf::core::Log::Level> &cats);
public:
	using Super::isMatchingLogFilter;
	bool isMatchingLogFilter(qf::core::Log::Level level, const char *file_name, const char *category) Q_DECL_OVERRIDE;
private:
	LogFilter m_logFilter;
};

void LoggerLogDevice::setCategories(const QMap<QString, qf::core::Log::Level> &cats)
{
	m_logFilter.categoriesTresholds = cats;
}

bool LoggerLogDevice::isMatchingLogFilter(qf::core::Log::Level level, const char *file_name, const char *category)
{
	Q_UNUSED(file_name)
	QString cat = category;
	qf::core::Log::Level treshold_level = m_logFilter.categoriesTresholds.value(cat, qf::core::Log::Level::Invalid);
	bool ok = (level <= treshold_level);
	//if(category && category[0])
	//	printf("%p %s %s:%d vs. %d -> %d\n", this, file_name, category, level, treshold_level, ok);
	return ok;
}

LoggerWidget::LoggerWidget(QWidget *parent)
	: Super(parent)
{
	m_logDevice =  LoggerLogDevice::install();
	//m_logDevice->setParent(this);
	//m_logDevice->setLogTreshold(qf::core::Log::LOG_WARN);

	addCategoryActions(tr("<empty>"), QString(), qf::core::Log::Level::Info);

	connect(m_logDevice, &qf::core::SignalLogDevice::logEntry, this, &LoggerWidget::addLogEntry);
}

LoggerWidget::~LoggerWidget()
{
	QF_SAFE_DELETE(m_logDevice);
}

Logging::LoggingPlugin *LoggerWidget::loggingPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Logging");
	return qobject_cast<Logging::LoggingPlugin *>(plugin);
}

void LoggerWidget::registerLogCategories()
{
	m_logDevice->setCategories(selectedLogCategories());
}


