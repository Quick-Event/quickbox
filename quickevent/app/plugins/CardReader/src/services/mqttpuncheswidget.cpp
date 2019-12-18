#include "mqttpuncheswidget.h"
#include "ui_mqttpuncheswidget.h"

namespace CardReader {
namespace services {

MqttPunchesWidget::MqttPunchesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::MqttPunchesWidget)
{
	setPersistentSettingsId("MqttPunchesWidget");
	ui->setupUi(this);
}

MqttPunchesWidget::~MqttPunchesWidget()
{
	delete ui;
}

}}
