#include "receiptssettingspage.h"
#include "ui_receiptssettingspage.h"
#include "receiptsplugin.h"
#include "receiptssettings.h"
#include "receiptsprinteroptionsdialog.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/core/log.h>

#include <QTimer>

namespace Receipts {

ReceiptsSettingsPage::ReceiptsSettingsPage(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::ReceiptsSettingsPage;
	ui->setupUi(this);
	m_caption = tr("Receipts");

	ui->cbxWhenRunnerNotFound->addItem(tr("Error info"), "ErrorInfo");
	ui->cbxWhenRunnerNotFound->addItem(tr("Receipt without name"), "ReceiptWithoutName");
	ui->cbxWhenRunnerNotFound->setCurrentIndex(0);

	loadReceptList();

	QTimer::singleShot(0, this, &ReceiptsSettingsPage::load);
}

ReceiptsSettingsPage::~ReceiptsSettingsPage()
{
	delete ui;
}

void ReceiptsSettingsPage::load()
{
	ReceiptsSettings settings;
	ui->chkAutoPrint->setChecked(settings.isAutoPrint());
	ui->chkThisReaderOnly->setChecked(settings.isThisReaderOnly());
	{
		auto *cbx = ui->cbxReceipt;
		QString path = settings.receiptPath();
		qfInfo() << "current receipt path:" << path;
		for (int i = 0; i < cbx->count(); ++i) {
			if(cbx->itemData(i).toString() == path) {
				cbx->setCurrentIndex(i);
				break;
			}
		}
		if(cbx->currentIndex() < 0) {
			cbx->setCurrentIndex(0);
			settings.setReceiptPath(cbx->currentData().toString());
		}
	}
	{
		auto *cbx = ui->cbxWhenRunnerNotFound;
		for (int i = 0; i < cbx->count(); ++i) {
			if(cbx->itemData(i).toString() == settings.whenRunnerNotFoundPrint()) {
				cbx->setCurrentIndex(i);
				break;
			}
		}
		if(cbx->currentIndex() < 0) {
			cbx->setCurrentIndex(0);
			settings.setWhenRunnerNotFoundPrint(cbx->currentData().toString());
		}
	}
}

void ReceiptsSettingsPage::save()
{
	ReceiptsSettings settings;
	settings.setAutoPrint(ui->chkAutoPrint->isChecked());
	settings.setThisReaderOnly(ui->chkThisReaderOnly->isChecked());
	settings.setReceiptPath(ui->cbxReceipt->currentData().toString());
	settings.setWhenRunnerNotFoundPrint(ui->cbxWhenRunnerNotFound->currentData().toString());
}

void ReceiptsSettingsPage::loadReceptList()
{
	ui->cbxReceipt->clear();
	auto *receipts_plugin = qf::qmlwidgets::framework::getPlugin<ReceiptsPlugin>();
	for(const auto &i : receipts_plugin->listReportFiles("receipts")) {
		qfDebug() << i.reportName << i.reportFilePath;
		ui->cbxReceipt->addItem(i.reportName, i.reportFilePath);
	}
	ui->cbxReceipt->setCurrentIndex(-1);
}

void ReceiptsSettingsPage::updateReceiptsPrinterLabel()
{
	ReceiptsSettings settings;
	ui->btPrinterOptions->setText(settings.printerCaption());
	if(settings.printerTypeEnum() == ReceiptsSettings::PrinterType::GraphicPrinter)
		ui->btPrinterOptions->setIcon(QIcon(":/quickevent/Receipts/images/graphic-printer.svg"));
	else
		ui->btPrinterOptions->setIcon(QIcon(":/quickevent/Receipts/images/character-printer.svg"));
}

void ReceiptsSettingsPage::onPrinterOptionsClicked()
{
	ReceiptsPrinterOptionsDialog dlg(this);
	//dlg.setPrinterOptions(getPlugin<ReceiptsPlugin>()->receiptsPrinter()->printerOptions());
	if(dlg.exec()) {
		//getPlugin<ReceiptsPlugin>()->setReceiptsPrinterOptions(dlg.printerOptions());
		updateReceiptsPrinterLabel();
	}
}

}
