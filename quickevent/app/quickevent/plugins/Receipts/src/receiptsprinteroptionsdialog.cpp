#include "receiptsprinteroptionsdialog.h"
#include "ui_receiptsprinteroptionsdialog.h"
#include "receiptssettings.h"

#include <QPrinterInfo>
#include <QPushButton>

enum { TabGraphicsPrinter = 0, TabTextPrinter };

ReceiptsPrinterOptionsDialog::ReceiptsPrinterOptionsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ReceiptsPrinterOptionsDialog)
{
	ui->setupUi(this);
	connect(ui->btGraphicsPrinter, &QPushButton::toggled, [this](bool checked) {
		if(checked)
			this->ui->stackedWidget->setCurrentIndex(TabGraphicsPrinter);
	});
	connect(ui->btCharacterPrinter, &QPushButton::toggled, [this](bool checked) {
		if(checked)
			this->ui->stackedWidget->setCurrentIndex(TabTextPrinter);
	});
	connect(ui->btCharacterPrinterLPT, &QPushButton::toggled, ui->cbxCharacterPrinterDevice, &QWidget::setEnabled);
	connect(ui->btCharacterPrinterDirectory, &QPushButton::toggled, ui->edCharacterPrinterDirectory, &QWidget::setEnabled);
	connect(ui->btCharacterPrinterNetwork, &QPushButton::toggled, ui->edCharacterPrinterUrl, &QWidget::setEnabled);
	connect(ui->btCharacterPrinterNetwork, &QPushButton::toggled, ui->chkCharacterPrinterUdpProtocol, &QWidget::setEnabled);
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

	load();
}

ReceiptsPrinterOptionsDialog::~ReceiptsPrinterOptionsDialog()
{
	delete ui;
}

void ReceiptsPrinterOptionsDialog::accept()
{
	save();
	QDialog::accept();
}

void ReceiptsPrinterOptionsDialog::loadPrinters()
{
	ui->cbxGraphicPrinter->addItems(QPrinterInfo::availablePrinterNames());
	QString def = QPrinterInfo::defaultPrinterName();
	ui->cbxGraphicPrinter->setCurrentText(def);
}

void ReceiptsPrinterOptionsDialog::load()
{
	loadPrinters();
	ReceiptsSettings settings;
	if(settings.printerTypeEnum() == ReceiptsSettings::PrinterType::GraphicPrinter) {
		this->ui->stackedWidget->setCurrentIndex(TabGraphicsPrinter);
		ui->btGraphicsPrinter->setChecked(true);
		ui->cbxGraphicPrinter->setCurrentText(settings.graphicsPrinterName());
	}
	else {
		this->ui->stackedWidget->setCurrentIndex(TabTextPrinter);
		ui->btCharacterPrinter->setChecked(true);
		ui->cbxCharacterPrinterDevice->setCurrentText(settings.characterPrinterDevice());
		ui->edCharacterPrinterDirectory->setText(settings.characterPrinterDirectory());
		ui->cbxCharacterPrinterModel->setCurrentText(settings.characterPrinterModel());
		ui->edCharacterPrinterLineLength->setValue(settings.characterPrinterLineLength());
		ui->chkCharacterPrinterGenerateControlCodes->setChecked(settings.isCharacterPrinterGenerateControlCodes());
		ui->edCharacterPrinterUrl->setText(settings.characterPrinterUrl());
		ui->chkCharacterPrinterUdpProtocol->setChecked(settings.isCharacterPrinterUdpProtocol());
		ui->cbxCharacterPrinterCodec->setCurrentText(settings.characterPrinterCodec());
		switch(settings.characterPrinterTypeEnum()) {
			case ReceiptsSettings::CharacterPrinteType::LPT:
				ui->btCharacterPrinterLPT->setChecked(true);
				break;
			case ReceiptsSettings::CharacterPrinteType::Directory:
				ui->btCharacterPrinterDirectory->setChecked(true);
				break;
			case ReceiptsSettings::CharacterPrinteType::Network:
				ui->btCharacterPrinterNetwork->setChecked(true);
				break;
		}
	}
}

void ReceiptsPrinterOptionsDialog::save()
{
	ReceiptsSettings settings;
	if(ui->btGraphicsPrinter->isChecked()) {
		settings.setPrinterType(ReceiptsSettings::printerTypeToString(ReceiptsSettings::PrinterType::GraphicPrinter));
		settings.setGraphicsPrinterName(ui->cbxGraphicPrinter->currentText());
	}
	else {
		settings.setPrinterType(ReceiptsSettings::printerTypeToString(ReceiptsSettings::PrinterType::CharacterPrinter));
		settings.setCharacterPrinterDevice(ui->cbxCharacterPrinterDevice->currentText());
		settings.setCharacterPrinterDirectory(ui->edCharacterPrinterDirectory->text());
		settings.setCharacterPrinterModel(ui->cbxCharacterPrinterModel->currentText());
		settings.setCharacterPrinterLineLength(ui->edCharacterPrinterLineLength->value());
		settings.setCharacterPrinterGenerateControlCodes(ui->chkCharacterPrinterGenerateControlCodes->isChecked());
		settings.setCharacterPrinterUrl(ui->edCharacterPrinterUrl->text());
		settings.setCharacterPrinterUdpProtocol(ui->chkCharacterPrinterUdpProtocol->isChecked());
		settings.setCharacterPrinterCodec(ui->cbxCharacterPrinterCodec->currentText());
		if(ui->btCharacterPrinterLPT->isChecked()) {
			settings.setCharacterPrinterType(ReceiptsSettings::characterPrinterTypeToString(ReceiptsSettings::CharacterPrinteType::LPT));
		}
		else if(ui->btCharacterPrinterDirectory->isChecked()) {
			settings.setCharacterPrinterType(ReceiptsSettings::characterPrinterTypeToString(ReceiptsSettings::CharacterPrinteType::Directory));
		}
		else {
			settings.setCharacterPrinterType(ReceiptsSettings::characterPrinterTypeToString(ReceiptsSettings::CharacterPrinteType::Network));
		}
	}
}

