
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "ui_dlgsettings.h"
#include "dlgsettings.h"
#include "cardreaderwidget.h"
#include "CardReader/cardreaderplugin.h"

//#include "theapp.h"

#include <qf/core/log.h>

#include <QSettings>
#include <QSqlDatabase>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>

//=================================================
//             DlgSettings
//=================================================
DlgSettings::DlgSettings(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::DlgSettings;
	ui->setupUi(this);
#if defined Q_OS_WIN
	for(int i=1; i<10; i++)
		ui->lstDevice->addItem(QString(\\COM%1).arg(i));
#elif defined Q_OS_UNIX
	for(int i=0; i<4; i++)
		ui->lstDevice->addItem(QString("/dev/ttyUSB0%1").arg(i));
#endif

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	load();
}

DlgSettings::~DlgSettings()
{
	delete ui;
}

void DlgSettings::accept()
{
	save();
	QDialog::accept();
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

void DlgSettings::load()
{
	QSettings settings;
	//settings.clear();
	settings.beginGroup(CardReader::CardReaderPlugin::SETTINGS_PREFIX);

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
	settings.endGroup();
	settings.endGroup();

	settings.endGroup();
}

void DlgSettings::save()
{
	QSettings settings;
	settings.beginGroup(CardReader::CardReaderPlugin::SETTINGS_PREFIX);

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

