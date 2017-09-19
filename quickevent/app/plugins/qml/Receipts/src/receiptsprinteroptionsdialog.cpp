#include "receiptsprinteroptionsdialog.h"
#include "ui_receiptsprinteroptionsdialog.h"
#include "receiptsprinteroptions.h"

#include <QPrinterInfo>
#include <QPushButton>

ReceiptsPrinterOptionsDialog::ReceiptsPrinterOptionsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ReceiptsPrinterOptionsDialog)
{
	ui->setupUi(this);
	connect(ui->btGraphicsPrinter, &QPushButton::toggled, [this](bool checked) {
		if(checked)
			this->ui->stackedWidget->setCurrentIndex(0);
	});
	connect(ui->btCharacterPrinter, &QPushButton::toggled, [this](bool checked) {
		if(checked)
			this->ui->stackedWidget->setCurrentIndex(1);
	});
	connect(ui->btCharacterPrinterLPT, &QPushButton::toggled, ui->cbxCharacterPrinterDevice, &QWidget::setEnabled);
	connect(ui->btCharacterPrinterDirectory, &QPushButton::toggled, ui->edCharacterPrinterDirectory, &QWidget::setEnabled);
	connect(ui->btCharacterPrinterNetwork, &QPushButton::toggled, ui->edCharacterPrinterAddress, &QWidget::setEnabled);
	connect(
		ui->cbxCharacterPrinterModel,
		static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		[this](int index) {
		switch(index) {
			case 0:
				ui->edCharacterPrinterLineLength->setValue(41);
				break;
			case 1:
				ui->edCharacterPrinterLineLength->setValue(40);
				break;
		}
	});

	loadPrinters();
}

ReceiptsPrinterOptionsDialog::~ReceiptsPrinterOptionsDialog()
{
	delete ui;
}

void ReceiptsPrinterOptionsDialog::loadPrinters()
{
	ui->cbxGraphicPrinter->addItems(QPrinterInfo::availablePrinterNames());
	QString def = QPrinterInfo::defaultPrinterName();
	ui->cbxGraphicPrinter->setCurrentText(def);
}

void ReceiptsPrinterOptionsDialog::setPrinterOptions(const ReceiptsPrinterOptions &opts)
{
	if(opts.printerType() == ReceiptsPrinterOptions::PrinterType::GraphicPrinter) {
		ui->btGraphicsPrinter->setChecked(true);
		ui->cbxGraphicPrinter->setCurrentText(opts.graphicsPrinterName());
	}
	else {
		ui->btCharacterPrinter->setChecked(true);
		ui->cbxCharacterPrinterDevice->setCurrentText(opts.characterPrinterDevice());
		ui->edCharacterPrinterDirectory->setText(opts.characterPrinterDirectory());
		ui->cbxCharacterPrinterModel->setCurrentText(opts.characterPrinterModel());
		ui->edCharacterPrinterLineLength->setValue(opts.characterPrinterLineLength());
		ui->chkCharacterPrinterGenerateControlCodes->setChecked(opts.isCharacterPrinterGenerateControlCodes());
		ui->edCharacterPrinterAddress->setText(opts.characterPrinterAddress());
		ui->cbxCharacterPrinterCodec->setCurrentText(opts.characterPrinterCodec());
		switch(opts.characterPrinterType()) {
			case ReceiptsPrinterOptions::CharacterPrinteType::LPT:
				ui->btCharacterPrinterLPT->setChecked(true);
				break;
			case ReceiptsPrinterOptions::CharacterPrinteType::Directory:
				ui->btCharacterPrinterDirectory->setChecked(true);
				break;
			case ReceiptsPrinterOptions::CharacterPrinteType::Network:
				ui->btCharacterPrinterNetwork->setChecked(true);
				break;
		}
	}
}

ReceiptsPrinterOptions ReceiptsPrinterOptionsDialog::printerOptions()
{
	ReceiptsPrinterOptions ret;
	if(ui->btGraphicsPrinter->isChecked()) {
		ret.setPrinterType((int)ReceiptsPrinterOptions::PrinterType::GraphicPrinter);
		ret.setGraphicsPrinterName(ui->cbxGraphicPrinter->currentText());
	}
	else {
		ret.setPrinterType((int)ReceiptsPrinterOptions::PrinterType::CharacterPrinter);
		ret.setCharacterPrinterDevice(ui->cbxCharacterPrinterDevice->currentText());
		ret.setCharacterPrinterDirectory(ui->edCharacterPrinterDirectory->text());
		ret.setCharacterPrinterModel(ui->cbxCharacterPrinterModel->currentText());
		ret.setCharacterPrinterLineLength(ui->edCharacterPrinterLineLength->value());
		ret.setCharacterPrinterGenerateControlCodes(ui->chkCharacterPrinterGenerateControlCodes->isChecked());
		ret.setCharacterPrinterAddress(ui->edCharacterPrinterAddress->text());
		ret.setCharacterPrinterCodec(ui->cbxCharacterPrinterCodec->currentText());
		if(ui->btCharacterPrinterLPT->isChecked()) {
			ret.setCharacterPrinterType(ReceiptsPrinterOptions::CharacterPrinteType::LPT);
		}
		else if(ui->btCharacterPrinterDirectory->isChecked()) {
			ret.setCharacterPrinterType(ReceiptsPrinterOptions::CharacterPrinteType::Directory);
		}
		else {
			ret.setCharacterPrinterType(ReceiptsPrinterOptions::CharacterPrinteType::Network);
		}
	}
	return ret;
}

