#include "runflagsdialog.h"
#include "ui_runflagsdialog.h"

#include "runstablemodel.h"

namespace Runs {

RunFlagsDialog::RunFlagsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RunFlagsDialog)
{
	ui->setupUi(this);
	connect(ui->cbxMisPunch, &QCheckBox::toggled, this, &RunFlagsDialog::updateState);
	connect(ui->cbxBadCheck, &QCheckBox::toggled, this, &RunFlagsDialog::updateState);
	connect(ui->cbxNotStart, &QCheckBox::toggled, this, &RunFlagsDialog::updateState);
	connect(ui->cbxNotFinish, &QCheckBox::toggled, this, &RunFlagsDialog::updateState);
	connect(ui->cbxDisqualifiedByOrganizer, &QCheckBox::toggled, this, &RunFlagsDialog::updateState);
	connect(ui->cbxNotCompeting, &QCheckBox::toggled, this, &RunFlagsDialog::updateStatusText);
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
	m_isDisqualified = table_row.value(QStringLiteral("runs.disqualified")).toBool();
	ui->cbxMisPunch->setChecked(table_row.value(QStringLiteral("runs.misPunch")).toBool());
	ui->cbxBadCheck->setChecked(table_row.value(QStringLiteral("runs.badCheck")).toBool());
	ui->cbxNotStart->setChecked(table_row.value(QStringLiteral("runs.notStart")).toBool());
	ui->cbxNotFinish->setChecked(table_row.value(QStringLiteral("runs.notFinish")).toBool());
	ui->cbxDisqualifiedByOrganizer->setChecked(table_row.value(QStringLiteral("runs.disqualifiedByOrganizer")).toBool());
	ui->cbxNotCompeting->setChecked(table_row.value(QStringLiteral("runs.notCompeting")).toBool());
	updateStatusText();
}

void RunFlagsDialog::save()
{
	qf::core::utils::TableRow &table_row = m_model->tableRowRef(m_row);
	table_row.setValue(QStringLiteral("runs.disqualified"), m_isDisqualified);
	table_row.setValue(QStringLiteral("runs.misPunch"), ui->cbxMisPunch->isChecked());
	table_row.setValue(QStringLiteral("runs.badCheck"), ui->cbxBadCheck->isChecked());
	table_row.setValue(QStringLiteral("runs.notStart"), ui->cbxNotStart->isChecked());
	table_row.setValue(QStringLiteral("runs.notFinish"), ui->cbxNotFinish->isChecked());
	table_row.setValue(QStringLiteral("runs.disqualifiedByOrganizer"), ui->cbxDisqualifiedByOrganizer->isChecked());
	table_row.setValue(QStringLiteral("runs.notCompeting"), ui->cbxNotCompeting->isChecked());
	m_model->postRow(m_row, qf::core::Exception::Throw);
}

void RunFlagsDialog::updateState()
{
	if (ui->cbxMisPunch->isChecked() ||
		ui->cbxBadCheck->isChecked() ||
		ui->cbxNotStart->isChecked() ||
		ui->cbxNotFinish->isChecked() ||
		ui->cbxDisqualifiedByOrganizer->isChecked())
	{
		m_isDisqualified = true;
	}
	else {
		m_isDisqualified = false;
	}
	updateStatusText();
}

void RunFlagsDialog::updateStatusText() {
	if (ui->cbxNotCompeting->isChecked()) {
		ui->runStatusText->setText("Not competing");
		return;
	}
	ui->runStatusText->setText(m_isDisqualified ? "Disqualified" : "OK");
}

} // namespace Runs
