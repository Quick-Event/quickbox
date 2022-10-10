#include "cardflagsdialog.h"
#include "ui_cardflagsdialog.h"

#include "runstablemodel.h"

namespace Runs {

CardFlagsDialog::CardFlagsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CardFlagsDialog)
{
	ui->setupUi(this);
	// make checkbox read-only
	ui->cbxCardInLentTable->setDisabled(true);
	connect(ui->cbxCardLent, &QCheckBox::toggled, this, &CardFlagsDialog::updateStatus);
	connect(ui->cbxCardReturned, &QCheckBox::toggled, this, &CardFlagsDialog::updateStatus);
}

CardFlagsDialog::~CardFlagsDialog()
{
	delete ui;
}

void CardFlagsDialog::load(RunsTableModel *model, int row)
{
	m_model = model;
	m_row = row;
	qf::core::utils::TableRow table_row = m_model->tableRow(m_row);
	ui->cbxCardLent->setChecked(table_row.value(QStringLiteral("runs.cardLent")).toBool());
	ui->cbxCardInLentTable->setChecked(table_row.value(QStringLiteral("cardInLentTable")).toBool());
	ui->cbxCardReturned->setChecked(table_row.value(QStringLiteral("runs.cardReturned")).toBool());
	updateStatus();
}

void CardFlagsDialog::save()
{
	qf::core::utils::TableRow &table_row = m_model->tableRowRef(m_row);
	table_row.setValue(QStringLiteral("runs.cardLent"), ui->cbxCardLent->isChecked());
	//table_row.setValue(QStringLiteral("cardInLentTable"), ui->cbxCardInLentTable->isChecked());
	table_row.setValue(QStringLiteral("runs.cardReturned"), ui->cbxCardReturned->isChecked());
	m_model->postRow(m_row, qf::core::Exception::Throw);
}

void CardFlagsDialog::updateStatus() {
	if (ui->cbxCardReturned->isChecked()) {
		ui->rentStatus->setText("Card rented and Returned");
		return;
	}
	if (!ui->cbxCardInLentTable->isChecked() && !ui->cbxCardLent->isChecked()) {
		ui->rentStatus->setText("Not a rented card");
		return;
	}
	ui->rentStatus->setText("Rented card");
}

} // namespace Runs
