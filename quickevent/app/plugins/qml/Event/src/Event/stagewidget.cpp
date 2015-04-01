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

	dataController()->setDocument(new StageDocument(this));
}

StageWidget::~StageWidget()
{
	delete ui;
}
