#include "eventdialogwidget.h"
#include "ui_eventdialogwidget.h"

EventDialogWidget::EventDialogWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::EventDialogWidget)
{
	setPersistentSettingsId("EventDialogWidget");
	ui->setupUi(this);

	QRegularExpression rx("[a-z0-9_]+");
	QValidator *validator = new QRegularExpressionValidator(rx, this);
	ui->ed_eventNameId->setValidator(validator);
}

EventDialogWidget::~EventDialogWidget()
{
	delete ui;
}

void EventDialogWidget::setEventId(const QString &event_id)
{
	ui->ed_eventNameId->setText(event_id);
}

QString EventDialogWidget::eventId() const
{
	return ui->ed_eventNameId->text();
}

void EventDialogWidget::loadParams(const QVariantMap &params)
{
	ui->ed_stageCount->setValue(params.value("event.stageCount").toInt());
	ui->ed_name->setText(params.value("event.name").toString());
	ui->ed_date->setText(params.value("event.date").toString());
	ui->ed_description->setText(params.value("event.description").toString());
	ui->ed_place->setText(params.value("event.place").toString());
	ui->ed_mainReferee->setText(params.value("event.mainReferee").toString());
	ui->ed_director->setText(params.value("event.director").toString());
}

QVariantMap EventDialogWidget::saveParams()
{
	QVariantMap ret;
	ret["event.stageCount"] = ui->ed_stageCount->value();
	ret["event.name"] = ui->ed_name->text();
	ret["event.date"] = ui->ed_date->text();
	ret["event.description"] = ui->ed_description->text();
	ret["event.place"] = ui->ed_place->text();
	ret["event.mainReferee"] = ui->ed_mainReferee->text();
	ret["event.director"] = ui->ed_director->text();
	return ret;
}
