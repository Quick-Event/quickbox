
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "cardreadersettingspage.h"
#include "ui_cardreadersettingspage.h"
#include "cardchecker.h"
//#include "cardreaderwidget.h"
#include "cardreaderplugin.h"
#include "cardreadersettings.h"

#include <siut/commport.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>

#include <QSettings>
#include <QSqlDatabase>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QMessageBox>

namespace CardReader {

//=================================================
//             DlgSettings
//=================================================
CardReaderSettingsPage::CardReaderSettingsPage(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::CardReaderSettingsPage;
	ui->setupUi(this);
	m_caption = tr("Card reader");
#if 0
#if defined Q_OS_WIN
	for(int i=1; i<10; i++)
		ui->lstDevice->addItem(QString("\\COM%1").arg(i));
#elif defined Q_OS_UNIX
	for(int i=0; i<4; i++)
		ui->lstDevice->addItem(QString("/dev/ttyUSB%1").arg(i));
#endif
#endif
	load();
}

CardReaderSettingsPage::~CardReaderSettingsPage()
{
	delete ui;
}

static void load_combo_text(QComboBox *cbx, const QVariant &val, bool init_current_index = true)
{
	int ix = cbx->findText(val.toString());
	if(ix >= 0) {
		cbx->setCurrentIndex(ix);
	}
	else {
		if(init_current_index) {
			cbx->setCurrentIndex(0);
		}
		else if(cbx->isEditable()) {
			cbx->lineEdit()->setText(val.toString());
		}
	}
}

void CardReaderSettingsPage::load()
{
	{
		ui->lstDevice->clear();
		QList<QSerialPortInfo> port_list = QSerialPortInfo::availablePorts();
		for(auto port : port_list) {
			//ui->lstDevice->addItem(QString("n:%1 l:%2").arg(port.portName()).arg(port.systemLocation()));
			ui->lstDevice->addItem(port.systemLocation());
		}

	}
	CardReaderSettings settings;
	load_combo_text(ui->lstDevice, settings.device(), false);
	load_combo_text(ui->lstBaudRate, settings.baudRate());
	load_combo_text(ui->lstDataBits, settings.dataBits());
	load_combo_text(ui->lstStopBits, settings.stopBits());
	load_combo_text(ui->lstParity, settings.parity());
	ui->chkShowRawComData->setChecked(settings.showRawComData());
	ui->chkDisableCRCCheck->setChecked(settings.disableCRCCheck());

	{
		auto *cbx = ui->cbxCardCheckType;
		for(auto *checker : qf::qmlwidgets::framework::getPlugin<CardReaderPlugin>()->cardCheckers()) {
			cbx->addItem(checker->caption(), checker->nameId());
		}
		for (int i = 0; i < cbx->count(); ++i) {
			if(cbx->itemData(i).toString() == settings.cardCheckType()) {
				cbx->setCurrentIndex(i);
				break;
			}
		}
		if(settings.cardCheckType().isEmpty())
			settings.setCardCheckType(cbx->currentData().toString());
	}
	{
		auto *cbx = ui->cbxReaderMode;
		cbx->addItem(tr("Readout"), "Readout");
		cbx->setItemData(0, CardReaderSettings::ReaderMode::Readout, Qt::UserRole + 1);
		cbx->setItemData(0, tr("Readout mode - default"), Qt::ToolTipRole);
		cbx->addItem(tr("Edit on punch"), "EditOnPunch");
		cbx->setItemData(0, CardReaderSettings::ReaderMode::EditOnPunch, Qt::UserRole + 1);
		cbx->setItemData(1, tr("Show Edit/Insert competitor dialog when SI Card is inserted into the reader station"), Qt::ToolTipRole);
		cbx->setCurrentIndex(-1);
		for (int i = 0; i < cbx->count(); ++i) {
			if(cbx->itemData(i).toString() == settings.readerMode()) {
				cbx->setCurrentIndex(i);
				break;
			}
		}
	}
}

void CardReaderSettingsPage::save()
{
	CardReaderSettings settings;
	settings.setDevice(ui->lstDevice->currentText());
	settings.setBaudRate(ui->lstBaudRate->currentText().toInt());
	settings.setDataBits(ui->lstDataBits->currentText().toInt());
	settings.setStopBits(ui->lstStopBits->currentText().toInt());
	settings.setParity(ui->lstParity->currentText());
	settings.setShowRawComData(ui->chkShowRawComData->isChecked());
	settings.setDisableCRCCheck(ui->chkDisableCRCCheck->isChecked());

	settings.setCardCheckType(ui->cbxCardCheckType->currentText());
	settings.setReaderMode(ui->cbxReaderMode->currentData().toString());
}


void CardReaderSettingsPage::on_btTestConnection_clicked()
{
	CardReaderSettings settings;
	QString device = settings.device();
	int baud_rate = settings.baudRate();
	int data_bits = settings.dataBits();
	int stop_bits = settings.stopBits();
	QString parity = settings.parity();
	siut::CommPort comport;
	if(comport.openComm(device, baud_rate, data_bits, parity, stop_bits > 1)) {
		QMessageBox::warning(this, tr("Message"), tr("Open serial port %1 success.").arg(device));
	}
	else {
		QString error_msg = comport.errorToUserHint();
		QMessageBox::warning(this, tr("Warning"), tr("Error open device %1 - %2").arg(device).arg(error_msg));
	}
}

}
