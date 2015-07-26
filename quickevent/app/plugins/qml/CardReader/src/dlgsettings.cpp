
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

//=================================================
//             DlgSettings
//=================================================
DlgSettings::DlgSettings(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::DlgSettings;
	ui->setupUi(this);

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

	settings.beginGroup("logging");
	load_combo_text(ui->lstLogLevel, settings, "level");
	ui->edCardLog->setText(settings.value("cardLog").toString());
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

	settings.beginGroup("logging");
	settings.setValue("level", ui->lstLogLevel->currentText());
	settings.setValue("cardLog", ui->edCardLog->text());
	settings.endGroup();

	settings.endGroup();
}

void DlgSettings::on_btCardLog_clicked()
{
	QString fn = QFileDialog::getSaveFileName(this, tr("Card log file"));
	if(!fn.isEmpty()) ui->edCardLog->setText(fn);
}

