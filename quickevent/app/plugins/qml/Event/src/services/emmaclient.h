#ifndef EMMACLIENT_H
#define EMMACLIENT_H

#include "service.h"

namespace services {

class EmmaClient : public services::Service
{
	Q_OBJECT

	using Super = services::Service;
public:
	EmmaClient(QObject *parent);

	//void run() override;
	//void stop() override;
private:
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void onCardChecked(const QVariantMap &data);
};

} // namespace services

#endif // EMMACLIENT_H
