#pragma once

#include "service.h"

class QTimer;

namespace Event {
namespace services {

class EmmaClientSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD2(bool, e, setE, xportTypeXML3, 0)
	QF_VARIANTMAP_FIELD(QString, e, setE, xportDir)
	QF_VARIANTMAP_FIELD(QString, f, setF, ileName)
	QF_VARIANTMAP_FIELD2(int, e, setE, xportIntervalSec, 0)
	QF_VARIANTMAP_FIELD2(bool, e, setE, xportStart, 0)
	QF_VARIANTMAP_FIELD2(bool, e, setE, xportFinish, 0)
public:
	EmmaClientSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class EmmaClient : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	EmmaClient(QObject *parent);

	//void run() override;
	//void stop() override;
	EmmaClientSettings settings() const {return EmmaClientSettings(m_settings);}

	static QString serviceName();

	void exportRadioCodes();
	void exportResultsIofXml3();
	void exportFinish();
	void exportStartList();
	bool preExport();
	void loadSettings() override;
private:
	bool createExportDir();
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void onCardChecked(const QVariantMap &data);
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
private:
	void onExportTimerTimeOut();
	void init();
private:
	QTimer *m_exportTimer = nullptr;
};

}}
