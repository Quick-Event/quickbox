
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "ui_cardreadersettingspage.h"
#include "cardreadersettingspage.h"
#include "cardreaderwidget.h"
#include "cardreaderplugin.h"

//#include "theapp.h"

#include <qf/core/log.h>

#include <QSettings>
#include <QSqlDatabase>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QSerialPortInfo>

//=================================================
//             DlgSettings
//=================================================
CardReaderSettingsPage::CardReaderSettingsPage(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::CardReaderSettingsPage;
	ui->setupUi(this);
	m_persistentId = CardReader::CardReaderPlugin::SETTINGS_PREFIX;
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

static void load_combo_text(QComboBox *cbx, const QSettings &settings, const QString &key, bool init_current_index = true)
{
	QVariant v = settings.value(key);
	int ix = cbx->findText(v.toString());
	if(ix >= 0) cbx->setCurrentIndex(ix);
	else {
		if(init_current_index) { cbx->setCurrentIndex(0); }
		else if(cbx->isEditable()) cbx->lineEdit()->setText(v.toString());
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
	QSettings settings;
	//settings.clear();
	settings.beginGroup(persistentId());

	settings.beginGroup("comm");
	settings.beginGroup("connection");
	load_combo_text(ui->lstDevice, settings, "device", false);
	load_combo_text(ui->lstBaudRate, settings, "baudRate");
	load_combo_text(ui->lstDataBits, settings, "dataBits");
	load_combo_text(ui->lstStopBits, settings, "stopBits");
	load_combo_text(ui->lstParity, settings, "parity");
	settings.endGroup();
	settings.beginGroup("debug");
	ui->chkShowRawComData->setChecked(settings.value("showRawComData").toBool());
	ui->chkDisableCRCCheck->setChecked(settings.value("disableCRCCheck").toBool());
}

void CardReaderSettingsPage::save()
{
	QSettings settings;
	settings.beginGroup(persistentId());

	settings.beginGroup("comm");
	settings.beginGroup("connection");
	settings.setValue("device", ui->lstDevice->currentText());
	settings.setValue("baudRate", ui->lstBaudRate->currentText());
	settings.setValue("dataBits", ui->lstDataBits->currentText());
	settings.setValue("stopBits", ui->lstStopBits->currentText());
	settings.setValue("parity", ui->lstParity->currentText());
	settings.endGroup();
	settings.beginGroup("debug");
	settings.setValue("showRawComData", ui->chkShowRawComData->isChecked());
	settings.setValue("disableCRCCheck", ui->chkDisableCRCCheck->isChecked());
	settings.endGroup();
	settings.endGroup();

	settings.endGroup();
}

