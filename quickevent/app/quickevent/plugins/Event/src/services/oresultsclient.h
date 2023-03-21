#ifndef ORESULTSCLIENT_H
#define ORESULTSCLIENT_H

#pragma once

#include "service.h"

class QTimer;
class QNetworkAccessManager;

namespace Event {
namespace services {

class OResultsClientSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD2(int, e, setE, xportIntervalSec, 60)
public:
	OResultsClientSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class OResultsClient : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	OResultsClient(QObject *parent);

	void run() override;
	void stop() override;
	OResultsClientSettings settings() const {return OResultsClientSettings(m_settings);}

	static QString serviceName();

	void exportResultsIofXml3();
	void exportStartListIofXml3();
	void loadSettings() override;
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	QString apiKey() const;
	void setApiKey(QString apiKey);
private:
	QTimer *m_exportTimer = nullptr;
	QNetworkAccessManager *m_networkManager = nullptr;
	const QString API_URL = "https://api.oresults.eu";
private:
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	void onExportTimerTimeOut();
	void init();
	void sendFile(QString name, QString request_path, QString file);
	void sendCompetitorChange(QString xml);
	void onCompetitorChanged(int competitor_id);
	QByteArray zlibCompress(QByteArray data);
};

}}

#endif // ORESULTSCLIENT_H
