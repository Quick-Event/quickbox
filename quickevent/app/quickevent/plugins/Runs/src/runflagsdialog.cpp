#include "runflagsdialog.h"
#include "ui_runflagsdialog.h"

#include "runstablemodel.h"

namespace Runs {

RunFlagsDialog::RunFlagsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RunFlagsDialog)
{
	ui->setupUi(this);
	connect(ui->cbxMisPunch, &QCheckBox::toggled, this, [this](bool on) { if(on) ui->cbxDisqualified->setChecked(true); });
	connect(ui->cbxBadCheck, &QCheckBox::toggled, this, [this](bool on) { if(on) ui->cbxDisqualified->setChecked(true); });
	connect(ui->cbxNotStart, &QCheckBox::toggled, this, [this](bool on) { if(on) ui->cbxDisqualified->setChecked(true); });
	connect(ui->cbxNotFinish, &QCheckBox::toggled, this, [this](bool on) { if(on) ui->cbxDisqualified->setChecked(true); });
}

RunFlagsDialog::~RunFlagsDialog()
{
	delete ui;
}

void RunFlagsDialog::load(RunsTableModel *model, int row)
{
	m_model = model;
	m_row = row;
	qf::core::utils::TableRow table_row = m_model->tableRow(m_row);
	ui->cbxDisqualified->setChecked(table_row.value(QStringLiteral("runs.disqualified")).toBool());
	ui->cbxMisPunch->setChecked(table_row.value(QStringLiteral("runs.misPunch")).toBool());
	ui->cbxBadCheck->setChecked(table_row.value(QStringLiteral("runs.badCheck")).toBool());
	ui->cbxNotCompeting->setChecked(table_row.value(QStringLiteral("runs.notCompeting")).toBool());
	ui->cbxNotStart->setChecked(table_row.value(QStringLiteral("runs.notStart")).toBool());
	ui->cbxNotFinish->setChecked(table_row.value(QStringLiteral("runs.notFinish")).toBool());
}

void RunFlagsDialog::save()
{
	qf::core::utils::TableRow &table_row = m_model->tableRowRef(m_row);
	table_row.setValue(QStringLiteral("runs.disqualified"), ui->cbxDisqualified->isChecked());
	table_row.setValue(QStringLiteral("runs.misPunch"), ui->cbxMisPunch->isChecked());
	table_row.setValue(QStringLiteral("runs.badCheck"), ui->cbxBadCheck->isChecked());
	table_row.setValue(QStringLiteral("runs.notCompeting"), ui->cbxNotCompeting->isChecked());
	table_row.setValue(QStringLiteral("runs.notStart"), ui->cbxNotStart->isChecked());
	table_row.setValue(QStringLiteral("runs.notFinish"), ui->cbxNotFinish->isChecked());
	m_model->postRow(m_row, qf::core::Exception::Throw);
}

} // namespace Runs
