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

