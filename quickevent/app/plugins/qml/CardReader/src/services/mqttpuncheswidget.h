#ifndef SERVICES_MQTTPUNCHESWIDGET_H
#define SERVICES_MQTTPUNCHESWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace services {

namespace Ui {
class MqttPunchesWidget;
}

class MqttPunchesWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit MqttPunchesWidget(QWidget *parent = nullptr);
	~MqttPunchesWidget();

private:
	Ui::MqttPunchesWidget *ui;
};


} // namespace services
#endif // SERVICES_MQTTPUNCHESWIDGET_H
