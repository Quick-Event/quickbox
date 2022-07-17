#include "plugin.h"
#include "application.h"
#include "mainwindow.h"

#include <qf/core/utils.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QQmlEngine>
#include <QTranslator>
#include <QFile>
#include <QDirIterator>

using namespace qf::qmlwidgets::framework;

QString Plugin::m_customReportsDir;

Plugin::Plugin(const QString &feature_id, QObject *parent)
	: QObject(parent)
{
	qfLogFuncFrame();
	m_featureId = feature_id;
}

Plugin::Plugin(QObject *parent)
	: QObject(parent)
{
	qfLogFuncFrame();
}

Plugin::~Plugin()
{
	qfLogFuncFrame() << this;
}

QString Plugin::findReportFile(const QString &report_file_path) const
{
	QStringList search_paths;
	if(!m_customReportsDir.isEmpty())
		search_paths << m_customReportsDir + '/' + m_featureId + "/qml/reports";
	search_paths << qmlReportsDir();
	for(const QString &dir : search_paths) {
		auto fn = dir + '/' + report_file_path;
		if(QFile::exists(fn))
			return fn;
	}
	qfError() << "Cannot find report file for feature id:" << m_featureId << "and file path:" << report_file_path;
	return {};
}

QList<Plugin::ReportFileInfo> Plugin::listReportFiles(const QString &report_dir) const
{
	QList<ReportFileInfo> report_files;
	QStringList search_paths;
	if(!m_customReportsDir.isEmpty())
		search_paths << m_customReportsDir + '/' + m_featureId + "/qml/reports";
	search_paths << qmlReportsDir();
	for(const QString &dir : search_paths) {
		QDirIterator it(dir + '/' + report_dir, QDirIterator::NoIteratorFlags);
		while (it.hasNext()) {
			it.next();
			QFileInfo fi = it.fileInfo();
			if(fi.isFile() && fi.suffix() == "qml") {
				ReportFileInfo i {
					.reportName = fi.baseName(),
					.reportFilePath = report_dir + '/' + fi.fileName()
				};
				if(!report_files.contains(i))
					report_files << i;
			}
		}
	}
	return report_files;
}

QQmlEngine *Plugin::qmlEngine()
{
	QQmlEngine *qe = Application::instance()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return nullptr);
	return qe;
}
