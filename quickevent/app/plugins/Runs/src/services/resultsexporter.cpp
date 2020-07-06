#include "resultsexporter.h"
#include "resultsexporterwidget.h"
#include "../runsplugin.h"

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

namespace Runs {
namespace services {

static Runs::RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

Event::EventPlugin *eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

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
	connect(this, &ResultsExporter::settingsChanged, this, &ResultsExporter::init, Qt::QueuedConnection);

}

QString ResultsExporter::serviceName()
{
	return QStringLiteral("ResultsExporter");
}

bool ResultsExporter::exportResults()
{
	ResultsExporterSettings ss = settings();
	if(!QDir().mkpath(ss.exportDir())) {
		qfError() << "Cannot create export dir:" << ss.exportDir();
		return false;
	}
	qfInfo() << "ResultsExporter export dir:" << ss.exportDir();
	if(ss.outputFormat() == static_cast<int>(ResultsExporterSettings::OutputFormat::CSOS)) {
		int current_stage = eventPlugin()->currentStageId();
		QString fn = ss.exportDir() + "/results-csos.txt";
		runsPlugin()->exportResultsCsosStage(current_stage, fn);
		return true;
	}
	if(ss.outputFormat() == static_cast<int>(ResultsExporterSettings::OutputFormat::IofXml3)) {
		int current_stage = eventPlugin()->currentStageId();
		QString fn = ss.exportDir() + "/results-csos.txt";
		runsPlugin()->exportResultsIofXml30Stage(current_stage, fn);
		return true;
	}
	else if(ss.outputFormat() == static_cast<int>(ResultsExporterSettings::OutputFormat::HtmlMulti)) {
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

			QStringList cmd_args = qf::core::Utils::parseProgramAndArgumentsList(cmd);
			proc->start(cmd_args.value(0), cmd_args.mid(1));
		}
		return true;
	}
	qfError() << "Unsupported output format:" << ss.outputFormat();
	return false;
}

void ResultsExporter::onExportTimerTimeOut()
{
	if(status() != Status::Running)
		return;
	if(!exportResults())
		stop();
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

	init();
}

void ResultsExporter::init()
{
	ResultsExporterSettings ss = settings();
	if(ss.exportIntervalSec() > 0) {
		m_exportTimer->setInterval(ss.exportIntervalSec() * 1000);
	}
	else {
		m_exportTimer->stop();
	}
}

}}
