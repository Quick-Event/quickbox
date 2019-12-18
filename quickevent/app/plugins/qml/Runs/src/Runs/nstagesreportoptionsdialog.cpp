#include "nstagesreportoptionsdialog.h"
#include "ui_nstagesreportoptionsdialog.h"

namespace Runs {

NStagesReportOptionsDialog::NStagesReportOptionsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NStagesReportOptionsDialog)
{
	ui->setupUi(this);
}

NStagesReportOptionsDialog::~NStagesReportOptionsDialog()
{
	delete ui;
}

int NStagesReportOptionsDialog::stagesCount() const
{
	return ui->edStagesCount->value();
}

void NStagesReportOptionsDialog::setStagesCount(int n)
{
	ui->edStagesCount->setValue(n);
}

int NStagesReportOptionsDialog::maxPlacesCount() const
{
	return ui->edMaxPlacesCount->value();
}

void NStagesReportOptionsDialog::setMaxPlacesCount(int n)
{
	ui->edMaxPlacesCount->setValue(n);
}

bool NStagesReportOptionsDialog::isExcludeDisqualified() const
{
	return ui->chkExcludeDisqualified->isChecked();
}

void NStagesReportOptionsDialog::setExcludeDisqualified(bool b)
{
	ui->chkExcludeDisqualified->setChecked(b);
}

} // namespace Runs
