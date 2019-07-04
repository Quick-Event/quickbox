#include "eventdialogwidget.h"
#include "ui_eventdialogwidget.h"

EventDialogWidget::EventDialogWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::EventDialogWidget)
{
	setPersistentSettingsId("EventDialogWidget");
	ui->setupUi(this);

	QRegularExpression rx("[A-Za-z][a-z0-9_]*"); // postgres schema cannot start with digit
	QValidator *validator = new QRegularExpressionValidator(rx, this);
	ui->ed_eventId->setValidator(validator);
}

EventDialogWidget::~EventDialogWidget()
{
	delete ui;
}

void EventDialogWidget::setEventId(const QString &event_id)
{
	ui->ed_eventId->setText(event_id);
}

QString EventDialogWidget::eventId() const
{
	return ui->ed_eventId->text();
}

void EventDialogWidget::setEventIdEditable(bool b)
{
	ui->ed_eventId->setReadOnly(!b);
}

void EventDialogWidget::loadParams(const QVariantMap &params)
{
	ui->ed_stageCount->setValue(params.value("stageCount").toInt());
	//ui->ed_currentStage->setValue(params.value("currentStageId").toInt());
	ui->ed_name->setText(params.value("name").toString());
	QDate date = params.value("date").toDate();
	if(!date.isValid())
		date = QDate::currentDate();
	ui->ed_date->setDate(date);
	QTime time = params.value("time").toTime();
	if(time.isValid())
		ui->ed_time->setTime(time);
	ui->ed_description->setText(params.value("description").toString());
	ui->ed_place->setText(params.value("place").toString());
	ui->ed_mainReferee->setText(params.value("mainReferee").toString());
	ui->ed_director->setText(params.value("director").toString());
	ui->ed_handicapLength->setValue(params.value("handicapLength").toInt());
	ui->cbxSportId->setCurrentIndex(params.value("sportId").toInt() - 1);
	ui->cbxDisciplineId->setCurrentIndex(params.value("disciplineId").toInt() - 1);
	if(ui->cbxDisciplineId->currentIndex() < 0)
		ui->cbxDisciplineId->setCurrentIndex(0);
	ui->ed_importId->setText(params.value("importId").toString());
	ui->ed_cardChecCheckTimeSec->setValue(params.value("cardChechCheckTimeSec").toInt());
}

QVariantMap EventDialogWidget::saveParams()
{
	QVariantMap ret;
	ret["stageCount"] = ui->ed_stageCount->value();
	//ret["currentStageId"] = ui->ed_currentStage->value();
	ret["name"] = ui->ed_name->text();
	ret["date"] = ui->ed_date->date();
	ret["time"] = ui->ed_time->time();
	ret["description"] = ui->ed_description->text();
	ret["place"] = ui->ed_place->text();
	ret["mainReferee"] = ui->ed_mainReferee->text();
	ret["director"] = ui->ed_director->text();
	ret["handicapLength"] = ui->ed_handicapLength->value();
	ret["sportId"] = ui->cbxSportId->currentIndex() + 1;
	ret["disciplineId"] = (ui->cbxDisciplineId->currentIndex() < 0)? 1: ui->cbxDisciplineId->currentIndex() + 1;
	ret["importId"] = ui->ed_importId->text().toInt();
	ret["cardChechCheckTimeSec"] = ui->ed_cardChecCheckTimeSec->value();
	return ret;
}
