#pragma once

#include <services/service.h>

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
	static QString serviceName();
	QString fileName() const {return m_fileName;}
	//static void setFileName(const QString &fn);
private:
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void onCardChecked(const QVariantMap &data);
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	void loadSettings() override;
private:
	QString m_fileName;
};

} // namespace services

