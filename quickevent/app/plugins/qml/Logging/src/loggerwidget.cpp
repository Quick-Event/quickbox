#include "loggerwidget.h"
#include "ui_loggerwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QComboBox>


LoggerWidget::LoggerWidget(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::LoggerWidget)
{
	ui->setupUi(this);
	m_logDevice =  qf::core::SignalLogDevice::install();
	m_logDevice->setParent(this);
	//m_logDevice->setLogTreshold(qf::core::Log::LOG_WARN);

	connect(m_logDevice, &qf::core::SignalLogDevice::logEntry, this, &LoggerWidget::onLogEntry);
	connect(ui->lstLogLevel, SIGNAL(activated(int)), this, SLOT(onLogLevelSet(int)));
	connect(ui->btClearLog, &QToolButton::clicked, ui->txtLog, &QPlainTextEdit::clear);
	//connect(ui->btClearLog, SIGNAL(clicked()), this, SLOT(clearLog()));
	ui->lstLogLevel->setCurrentText("Info");
}

LoggerWidget::~LoggerWidget()
{
	delete ui;
}

Logging::LoggingPlugin *LoggerWidget::loggingPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Logging");
	return qobject_cast<Logging::LoggingPlugin *>(plugin);
}

void LoggerWidget::onLogEntry(const qf::core::LogEntryMap &log_entry)
{
	/// !!!! do not call log in log handler !!!
	//qf::core::LogEntryMap log_entry(log_entry);
	QString msg = log_entry.message();
	QString filter_str = ui->edDomainFilter->text();
	if(!filter_str.isEmpty() && !msg.contains(filter_str, Qt::CaseInsensitive))
		return;
	qf::core::Log::Level level = log_entry.level();
	QString log_level_str = qf::core::Log::levelName(level);

	QString color;
	switch(level) {
	case qf::core::Log::Level::Error: color = "#FF0000"; break;
	case qf::core::Log::Level::Warning: color = "#FF00FF"; break;
	case qf::core::Log::Level::Info: color = "#0000FF"; break;
	default: color = "#000000"; break;
	}
	QString message = "<font color=\"%4\">&lt;%1&gt;[%2]%5 %3<font>";
	message = message
			  .arg(log_level_str)
			  .arg(qf::core::LogDevice::moduleFromFileName(log_entry.file()))
			  .arg(msg)
			  .arg(color);
	QString category = log_entry.category().isEmpty()? QString(): '(' + log_entry.category() + ')';
	message = message.arg(category);

	ui->txtLog->appendHtml(message);
	if(level <= qf::core::Log::Level::Warning) {
		loggingPlugin()->setLogDockVisible(true);
	}
}

void LoggerWidget::onLogLevelSet(int ix)
{
	//qfInfo() << "new log level:" << qf::core::Log::levelName(qf::core::Log::Level(ix));
	m_logDevice->setLogTreshold(qf::core::Log::Level(ix + 1));
	//QString log_level_str = qf::core::Log::levelName(m_logDevice->logTreshold());
	//qfError() << "LoggerWidget::onLogLevelSet to:" << log_level_str;
	//qfWarning() << "LoggerWidget::onLogLevelSet to:" << log_level_str;
	//qfInfo() << "LoggerWidget::onLogLevelSet to:" << log_level_str;
	//qfDebug() << "LoggerWidget::onLogLevelSet to:" << log_level_str;
}

