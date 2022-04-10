#include "stagewidget.h"
#include "stagedocument.h"
#include "ui_stagewidget.h"

using namespace Event;

StageWidget::StageWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::StageWidget)
{
	setPersistentSettingsId("StageWidget");
	ui->setupUi(this);

	setTitle(tr("Stage"));
	setWindowTitle(tr("Edit Stage"));
	dataController()->setDocument(new StageDocument(this));
}

StageWidget::~StageWidget()
{
	delete ui;
}

bool StageWidget::load(const QVariant &id, int mode)
{
	bool ok  = Super::load(id, mode);
	if(ok) {
		QDateTime dt;
		qf::core::model::DataDocument *doc = dataDocument();
		const auto START_DATE_TIME = QStringLiteral("startDateTime");
		dt = doc->value(START_DATE_TIME).toDateTime();
		/*
		if(!dt.isValid()) {
			// try old DB version
			QDate d = doc->value(QStringLiteral("startDate")).toDate();
			QTime t = doc->value(QStringLiteral("startTime")).toTime();
			dt = QDateTime(d, t);
		}
		*/
		ui->dateEdit->setDate(dt.date());
		ui->timeEdit->setTime(dt.time());
	}
	return ok;
}

bool StageWidget::saveData()
{
	QDate d = ui->dateEdit->date();
	QTime t = ui->timeEdit->time();
	QDateTime dt(d, t);
	qf::core::model::DataDocument *doc = dataDocument();
	const auto START_DATE_TIME = QStringLiteral("startDateTime");
	doc->setValue(START_DATE_TIME, dt);
	/*
	if(doc->isValidFieldName(START_DATE_TIME)) {
		doc->setValue(START_DATE_TIME, dt);
	}
	else {
		// old DB version
		doc->setValue(QStringLiteral("startDate"), d);
		doc->setValue(QStringLiteral("startTime"), t);
	}
	*/
	return Super::saveData();
}
