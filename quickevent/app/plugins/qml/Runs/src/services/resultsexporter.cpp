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
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

namespace services {

ResultsExporter::ResultsExporter(QObject *parent)
	: Super(ResultsExporter::serviceName(), parent)
{
	m_exportTimer = new QTimer(this);
	connect(m_exportTimer, &QTimer::timeout, this, &ResultsExporter::onExportTimerTimeOut);
	connect(this, &ResultsExporter::statusChanged, [this](Status status) {
		if(status == Status::Running) {
			if(settings().exportIntervalSec() > 0) {
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
/*
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
*/
void ResultsExporter::onExportTimerTimeOut()
{
	if(status() != Status::Running)
		return;
	ResultsExporterSettings ss = settings();
	if(!QDir().mkpath(ss.exportDir())) {
		qfError() << "Cannot create export dir:" << ss.exportDir();
		stop();
		return;
	}
	qfInfo() << "ResultsExporter export dir:" << ss.exportDir();
	quickevent::core::exporters::StageResultsHtmlExporter exp;
	exp.setOutDir(ss.exportDir());
	exp.generateHtml();

	QString cmd = ss.whenFinishedRunCmd();
	if(!cmd.isEmpty()) {
		qfInfo() << "Starting process:" << cmd;
		QProcess *proc = new QProcess();
		connect(proc, &QProcess::readyReadStandardOutput, [proc]() {
			QByteArray ba = proc->readAllStandardOutput();
			qfInfo().noquote() << "PROC stdout:" << ba;
		});
		connect(proc, &QProcess::readyReadStandardError, [proc]() {
			QByteArray ba = proc->readAllStandardError();
			qfWarning().noquote() << "PROC stderr:" << ba;
		});
		connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [proc](int exit_code, QProcess::ExitStatus exit_status) {
			if(exit_status == QProcess::ExitStatus::CrashExit)
				qfError() << "PROC crashed";
			else
				qfInfo() << "PROC finished with exit code:" << exit_code;
			proc->deleteLater();
		});

		proc->start(cmd);
	}
}

qf::qmlwidgets::framework::DialogWidget *ResultsExporter::createDetailWidget()
{
	auto *w = new ResultsExporterWidget();
	return w;
}

void ResultsExporter::loadSettings()
{
	Super::loadSettings();
	ResultsExporterSettings ss = settings();

	if(ss.exportDir().isEmpty()) {
		ss.setExportDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/quickevent/results");
		setSettings(ss);
	}

	if(ss.exportIntervalSec() > 0) {
		m_exportTimer->setInterval(ss.exportIntervalSec() * 1000);
	}
	else {
		m_exportTimer->stop();
	}
}

} // namespace services
