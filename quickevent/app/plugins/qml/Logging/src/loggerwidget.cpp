#include "loggerwidget.h"
#include "ui_loggerwidget.h"

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

	connect(m_logDevice, &qf::core::SignalLogDevice::logEntry, this, &LoggerWidget::onLogEntry, Qt::QueuedConnection);
	connect(ui->lstLogLevel, SIGNAL(activated(int)), this, SLOT(onLogLevelSet(int)));
	connect(ui->btClearLog, &QToolButton::clicked, ui->txtLog, &QPlainTextEdit::clear);
	//connect(ui->btClearLog, SIGNAL(clicked()), this, SLOT(clearLog()));
	ui->lstLogLevel->setCurrentText("Info");
}

LoggerWidget::~LoggerWidget()
{
	delete ui;
}

void LoggerWidget::onLogEntry(const QVariantMap &log_entry)
{
	/// !!!! do not call log in log handler !!!
	qf::core::LogEntryMap em(log_entry);
	QString msg = em.message();
	QString filter_str = ui->edDomainFilter->text();
	if(!filter_str.isEmpty() && !msg.contains(filter_str, Qt::CaseInsensitive))
		return;
	qf::core::Log::Level level = em.level();
	QString log_level_str = qf::core::Log::levelName(level);

	QString color;
	switch(level) {
	case qf::core::Log::LOG_ERR: color = "#FF0000"; break;
	case qf::core::Log::LOG_WARN: color = "#FF00FF"; break;
	case qf::core::Log::LOG_INFO: color = "#0000FF"; break;
	default: color = "#000000"; break;
	}
	QString message = "<font color=\"%4\">&lt;%1&gt;[%2] %3<font>";
	message = message.arg(log_level_str).arg(em.domain()).arg(msg).arg(color);

	ui->txtLog->appendHtml(message);
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

