#include "competitorwidget.h"
#include "ui_competitorwidget.h"

#include "competitordocument.h"

CompetitorWidget::CompetitorWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorWidget)
{
	setPersistentSettingsId("CompetitorWidget");
	ui->setupUi(this);

	qf::qmlwidgets::ForeignKeyComboBox *cbx = ui->cbxClass;

	cbx->setReferencedTable("classes");
	cbx->setReferencedField("id");
	cbx->setReferencedCaptionField("name");

	dataController()->setDocument(new CompetitorDocument(this));
}

CompetitorWidget::~CompetitorWidget()
{
	delete ui;
}
