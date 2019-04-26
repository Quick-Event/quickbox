#include "classdefwidget.h"
#include "ui_classdefwidget.h"

#include "classdefdocument.h"

ClassDefWidget::ClassDefWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::ClassDefWidget)

{
	setPersistentSettingsId("ClassDefWidget");
	ui->setupUi(this);
	setWindowTitle(tr("Edit class"));

	// set class start time read only, class start cannot be set this way in drawing tool
	ui->edStartTime->setReadOnly(true);

	dataController()->setDocument(new ClassDefDocument(this));
}

ClassDefWidget::~ClassDefWidget()
{
}

bool ClassDefWidget::load(const QVariant &id, int mode)
{
	bool ok = Super::load(id, mode);

	setTitle(tr("Class %1").arg(dataDocument()->value("classes.name").toString()));

	return ok;
}

