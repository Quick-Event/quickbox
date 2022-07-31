
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "ui_cardreadersettingspage.h"
#include "cardreadersettingspage.h"
#include "cardreaderwidget.h"
#include "cardreaderplugin.h"
#include "cardreadersettings.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>

#include <QSettings>
#include <QSqlDatabase>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QSerialPortInfo>

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
}


void CardReaderSettingsPage::on_btTestConnection_clicked()
{
	/*
	QSettings settings;
	settings.beginGroup(settingsDir());
	settings.beginGroup("comm");
	settings.beginGroup("connection");
	QString device = settings.value("device", "").toString();
	int baud_rate = settings.value("baudRate", 38400).toInt();
	int data_bits = settings.value("dataBits", 8).toInt();
	int stop_bits = settings.value("stopBits", 1).toInt();
	QString parity = settings.value("parity", "none").toString();
	if(!commPort()->openComm(device, baud_rate, data_bits, parity, stop_bits > 1)) {
	*/
}

}
