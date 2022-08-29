
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
#include <siut/sidevicedriver.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>

#include <QLineEdit>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>

namespace CardReader {

CardReaderSettingsPage::CardReaderSettingsPage(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::CardReaderSettingsPage;
	ui->setupUi(this);
	m_caption = tr("Card reader");
	{
		auto *cbx = ui->cbxCardCheckType;
		for(auto *checker : qf::qmlwidgets::framework::getPlugin<CardReaderPlugin>()->cardCheckers()) {
			cbx->addItem(checker->caption(), checker->nameId());
		}
	}
	{
		auto *cbx = ui->cbxReaderMode;
		cbx->addItem(tr("Readout"), "Readout");
		//cbx->setItemData(0, CardReaderSettings::ReaderMode::Readout, Qt::UserRole + 1);
		cbx->setItemData(0, tr("Readout mode - default"), Qt::ToolTipRole);
		cbx->addItem(tr("Edit on punch"), "EditOnPunch");
		//cbx->setItemData(0, CardReaderSettings::ReaderMode::EditOnPunch, Qt::UserRole + 1);
		cbx->setItemData(1, tr("Show Edit/Insert competitor dialog when SI Card is inserted into the reader station"), Qt::ToolTipRole);
	}

	QTimer::singleShot(0, this, &CardReaderSettingsPage::load);
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
	ui->chkShowRawComData->setChecked(settings.isShowRawComData());
	ui->chkDisableCRCCheck->setChecked(settings.isDisableCRCCheck());

	{
		auto *cbx = ui->cbxCardCheckType;
		cbx->setCurrentIndex(-1);
		for (int i = 0; i < cbx->count(); ++i) {
			if(cbx->itemData(i).toString() == settings.cardCheckType()) {
				cbx->setCurrentIndex(i);
				break;
			}
		}
		if(cbx->currentIndex() < 0) {
			cbx->setCurrentIndex(0);
			settings.setCardCheckType(cbx->currentData().toString());
		}
	}
	{
		auto *cbx = ui->cbxReaderMode;
		for (int i = 0; i < cbx->count(); ++i) {
			cbx->setCurrentIndex(-1);
			if(cbx->itemData(i).toString() == settings.readerMode()) {
				cbx->setCurrentIndex(i);
				break;
			}
		}
		if(cbx->currentIndex() < 0) {
			cbx->setCurrentIndex(0);
			settings.setReaderMode(cbx->currentData().toString());
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

	settings.setCardCheckType(ui->cbxCardCheckType->currentData().toString());
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
	auto *comport = new siut::CommPort();
	if(comport->openComm(device, baud_rate, data_bits, parity, stop_bits > 1)) {
		QMessageBox::warning(this, tr("Message"), tr("Open serial port %1 success.").arg(device));
		auto *sidriver = new siut::DeviceDriver();
		connect(comport, &siut::CommPort::readyRead, this, [comport, sidriver]() {
			QByteArray ba = comport->readAll();
			sidriver->processData(ba);
		});
		connect(sidriver, &siut::DeviceDriver::dataToSend, comport, &siut::CommPort::sendData);
		siut::SiTaskStationConfig *cmd = new siut::SiTaskStationConfig();
		connect(cmd, &siut::SiTaskStationConfig::finished, this, [this, comport, sidriver](bool ok, QVariant result) {
			if(ok) {
				siut::SiStationConfig cfg(result.toMap());
				QString msg = cfg.toString();
				QMessageBox::information(this, tr("Information"), tr("SI reader config:%1").arg(msg));
			}
			else {
				QMessageBox::warning(this, tr("Warning"), tr("Device %1 is not SI reader").arg(comport->portName()));
			}
			sidriver->deleteLater();
			comport->deleteLater();
		}, Qt::QueuedConnection);
		sidriver->setSiTask(cmd);
	}
	else {
		QString error_msg = comport->errorToUserHint();
		QMessageBox::warning(this, tr("Warning"), tr("Error open device %1 - %2").arg(device, error_msg));
		delete comport;
	}
}

}
