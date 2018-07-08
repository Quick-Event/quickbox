#ifndef SERVICES_SERVICEWIDGET_H
#define SERVICES_SERVICEWIDGET_H

#include "service.h"

#include <QWidget>

namespace services {

namespace Ui {
class ServiceWidget;
}

class ServiceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ServiceWidget(QWidget *parent = 0);
	~ServiceWidget();

	void setStatus(Service::Status st);
	void setName(const QString &n);
	void setMessage(const QString &m);

	Q_SIGNAL void setRunningRequest(bool play);
private:
	void onBtPlayToggled(bool on);
	private:
	Ui::ServiceWidget *ui;
};


} // namespace services
#endif // SERVICES_SERVICEWIDGET_H
