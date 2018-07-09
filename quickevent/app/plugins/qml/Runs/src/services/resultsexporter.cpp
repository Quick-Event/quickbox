#include "resultsexporter.h"
#include "resultsexporterwidget.h"

#include <Event/eventplugin.h>

#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/exporters/stageresultshtmlexporter.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

namespace services {

static auto KEY_EXPORT_DIR = QStringLiteral("exportDir");
static auto KEY_EXPORT_INTERVAL = QStringLiteral("exportInterval");
static auto KEY_WHEN_FINSHED_RUN_CMD = QStringLiteral("whenFinishedRunCmd");

ResultsExporter::ResultsExporter(QObject *parent)
	: Super(ResultsExporter::serviceName(), parent)
{
	m_exportTimer = new QTimer(this);
	connect(m_exportTimer, &QTimer::timeout, this, &ResultsExporter::onExportTimerTimeOut);
	connect(this, &ResultsExporter::statusChanged, [this](Status status) {
		if(status == Status::Running) {
			if(m_exportIntervalSec > 0) {
				onExportTimerTimeOut();
				m_exportTimer->start();
			}
		}
		else {
			m_exportTimer->stop();
		}
	});
}

QString ResultsExporter::serviceName()
{
	return QStringLiteral("ResultsExporter");
}

void ResultsExporter::setExportDir(const QString &s)
{
	QSettings settings;
	settings.beginGroup(settingsGroup());
	settings.setValue(KEY_EXPORT_DIR, s);
	m_exportDir = s;
}

void ResultsExporter::setExportIntervalSec(int sec)
{
	QSettings settings;
	settings.beginGroup(settingsGroup());
	settings.setValue(KEY_EXPORT_INTERVAL, sec);
	m_exportIntervalSec = sec;
}

void ResultsExporter::setWhenFinishedRunCmd(const QString &s)
{
	QSettings settings;
	settings.beginGroup(settingsGroup());
	settings.setValue(KEY_WHEN_FINSHED_RUN_CMD, s);
	m_whenFinishedRunCmd = s;
}

void ResultsExporter::onExportTimerTimeOut()
{
	if(status() != Status::Running)
		return;
	qfInfo() << "generate results to:" << exportDir();
	quickevent::core::exporters::StageResultsHtmlExporter exp;
	exp.setOutDir(exportDir());
	exp.generateHtml();
}

qf::qmlwidgets::framework::DialogWidget *ResultsExporter::createDetailWidget()
{
	auto *w = new ResultsExporterWidget();
	return w;
}

void ResultsExporter::loadSettings()
{
	QSettings settings;
	settings.beginGroup(settingsGroup());
	m_exportDir = settings.value(KEY_EXPORT_DIR).toString();
	if(m_exportDir.isEmpty()) {
		m_exportDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/quickevent/results";
	}
	QDir().mkpath(m_exportDir);
	qfInfo() << "ResultsExporter export dir:" << exportDir();

	m_exportIntervalSec = settings.value(KEY_EXPORT_INTERVAL).toInt();
	if(m_exportIntervalSec > 0) {
		m_exportTimer->setInterval(m_exportIntervalSec * 1000);
	}
	else {
		m_exportTimer->stop();
	}

	m_whenFinishedRunCmd = settings.value(KEY_WHEN_FINSHED_RUN_CMD).toString();
}

} // namespace services
