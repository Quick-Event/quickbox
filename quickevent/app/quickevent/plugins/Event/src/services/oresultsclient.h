#ifndef ORESULTSCLIENT_H
#define ORESULTSCLIENT_H

#pragma once

#include "service.h"

#include <QNetworkAccessManager>

class QTimer;

namespace Event {
namespace services {

class OResultsClientSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD(QString, a, setA, piKey)
	QF_VARIANTMAP_FIELD2(int, e, setE, xportIntervalSec, 15)
public:
	OResultsClientSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class OResultsClient : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	OResultsClient(QObject *parent);

	//void run() override;
	//void stop() override;
	OResultsClientSettings settings() const {return OResultsClientSettings(m_settings);}

	static QString serviceName();

	void exportResultsIofXml3();
	void exportStartListIofXml3();
	void loadSettings() override;
private:
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	void onExportTimerTimeOut();
	void init();
	QTimer *m_exportTimer = nullptr;
	QNetworkAccessManager *m_networkManager = nullptr;
	void sendFile(QString name, QString request_path, QString file);
};

}}

#endif // ORESULTSCLIENT_H
