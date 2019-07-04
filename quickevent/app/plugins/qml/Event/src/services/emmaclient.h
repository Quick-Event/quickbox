#ifndef EMMACLIENT_H
#define EMMACLIENT_H

#include "service.h"

namespace services {

class EmmaClientSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD(QString, e, setE, xportDir)
	QF_VARIANTMAP_FIELD(QString, f, setF, ileName)

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
private:
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void onCardChecked(const QVariantMap &data);
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
};

} // namespace services

#endif // EMMACLIENT_H
