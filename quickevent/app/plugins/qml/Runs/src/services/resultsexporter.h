#pragma once

#include <services/service.h>

class QTimer;

namespace services {

class ResultsExporter : public services::Service
{
	Q_OBJECT

	using Super = services::Service;
public:
	//static const char *SETTINGS_GROUP;
	static const char *SETTING_KEY_FILE_NAME;
public:
	ResultsExporter(QObject *parent);

	//void run() override;
	//void stop() override;
	void loadSettings() override;
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;

	static QString serviceName();

	QString exportDir() const {return m_exportDir;}
	void setExportDir(const QString &s);

	int exportIntervalSec() const {return m_exportIntervalSec;}
	void setExportIntervalSec(int sec);

	QString whenFinishedRunCmd() const {return m_whenFinishedRunCmd;}
	void setWhenFinishedRunCmd(const QString &s);
private:
	void onExportTimerTimeOut();
private:
	QString m_exportDir;
	int m_exportIntervalSec;
	QString m_whenFinishedRunCmd;

	QTimer *m_exportTimer = nullptr;
};

} // namespace services

