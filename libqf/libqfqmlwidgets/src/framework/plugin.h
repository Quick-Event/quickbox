#ifndef QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H
#define QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H

#include "../qmlwidgetsglobal.h"
#include "application.h"

#include <qf/core/utils.h>

#include <QObject>

class QQmlEngine;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT Plugin : public QObject
{
	Q_OBJECT
public:
	explicit Plugin(const QString &feature_id, QObject *parent = nullptr);
	explicit Plugin(QObject *parent = nullptr);
	~Plugin() Q_DECL_OVERRIDE;

	static void setReportsDir(const QString &dir) { m_reportsDir = dir; }
	static QString reportsDir() { return m_reportsDir; }

	QString homeDir() const { return qf::qmlwidgets::framework::Application::instance()->pluginDataDir() + '/' +  featureId(); }
	QString qmlDir() const { return homeDir() + "/qml"; }
	QString qmlReportsDir() const { return qmlDir() + "/reports"; }
	QString featureId() const { return m_featureId; }
	//QString settingsDir() const { return "plugin/" +  featureId(); }
	QString findReportFile(const QString &report_file_path) const;
	struct QFQMLWIDGETS_DECL_EXPORT ReportFileInfo
	{
		QString reportName;
		QString reportFilePath;

		bool operator==(const ReportFileInfo &o) const {
			return reportName == o.reportName && reportFilePath == o.reportFilePath;
		}
	};
	QList<ReportFileInfo> listReportFiles(const QString &report_dir) const;

	Q_SIGNAL void installed();
private:
	QString m_featureId;
	static QString m_reportsDir;
};

}}}

#endif
