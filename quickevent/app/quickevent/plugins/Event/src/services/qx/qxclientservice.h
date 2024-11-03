#pragma once

#include "../service.h"

namespace Event::services::qx {

class RootNode;

class QxClientServiceSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD(QString, e, setE, xchangeServerUrl)
	//QF_VARIANTMAP_FIELD(QString, e, setE, ventPath)
	//QF_VARIANTMAP_FIELD(QString, a, setA, piKey)
public:
	QxClientServiceSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}

	QString xchgKey() const;
};

class QxClientService : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	QxClientService(QObject *parent);

	static QString serviceId();
	QString serviceDisplayName() const override;

	void run() override;
	void stop() override;
	QxClientServiceSettings settings() const {return QxClientServiceSettings(m_settings);}

	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
private:
	void loadSettings() override;
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
};

}
