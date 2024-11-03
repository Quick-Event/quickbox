#include "serviceswidget.h"
#include "service.h"
#include "servicewidget.h"

#include <QHBoxLayout>
#include <QScrollArea>

namespace Event {
namespace services {

ServicesWidget::ServicesWidget(QWidget *parent)
	: QWidget(parent)
{
	new QHBoxLayout(this);
}

void ServicesWidget::reload()
{
	if(m_centralWidget)
		delete m_centralWidget;
	m_centralWidget = new QScrollArea();
	QLayout *ly = layout();
	ly->addWidget(m_centralWidget);

	QVBoxLayout *ly2 = new QVBoxLayout(m_centralWidget);
#if QT_VERSION_MAJOR >= 6
	ly2->setContentsMargins({});
#else
	ly2->setMargin(0);
#endif
	ly->setSpacing(0);

	for (int i = 0; i < Service::serviceCount(); ++i) {
		Service *svc = Service::serviceAt(i);

		ServiceWidget *sw = new ServiceWidget();
		sw->setStatus(svc->status());
		connect(svc, &Service::statusChanged, sw, &ServiceWidget::setStatus);
		sw->setServiceId(svc->serviceId(), svc->serviceDisplayName());
		sw->setMessage(svc->statusMessage());
		connect(svc, &Service::statusMessageChanged, sw, &ServiceWidget::setMessage);

		connect(sw, &ServiceWidget::setRunningRequest, svc, &Service::setRunning);

		ly2->addWidget(sw);
	}
	ly2->addStretch();
}

}}
