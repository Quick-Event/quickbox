
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "ui_dlgsettings.h"
#include "dlgsettings.h"

#include "theapp.h"

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

	foreach(QString s, QSqlDatabase::drivers()) ui->lstSqlDriver->addItem(s, s);
	{
		QDir d(QCoreApplication::applicationDirPath() + "/divers/qsicli/scripts/extensions");
		foreach(QString s, d.entryList(QDir::Files)) {
			//qfInfo() << s;
			if(s.section('.', -1, -1) == "js") ui->lstExtensionsCurrentName->addItem(s, "extensions." + s.section('.', 0, -2));
		}
	}

	load();
}

DlgSettings::~DlgSettings()
{
	delete ui;
}

void DlgSettings::accept()
{
	save();
	theApp()->closeCardLog();
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

static void load_combo_data(QComboBox *cbx, const QSettings &settings, const QString &key, bool init_current_index = true)
{
	QVariant v = settings.value(key);
	int ix = cbx->findData(v);
	if(ix >= 0) cbx->setCurrentIndex(ix);
	else {
		if(init_current_index) { cbx->setCurrentIndex(0); }
		else if(cbx->isEditable()) cbx->lineEdit()->setText(QString());
	}
}

void DlgSettings::load()
{
	QSettings settings;
	//settings.clear();
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

	settings.beginGroup("sql");
	settings.beginGroup("connection");
	load_combo_data(ui->lstSqlDriver, settings, "driver");
	ui->edSqlHost->setText(settings.value("host").toString());
	ui->edSqlPort->setValue(settings.value("port").toInt());
	ui->edSqlUser->setText(settings.value("user").toString());
	ui->edSqlPassword->setText(settings.value("password").toString());
	ui->edSqlDatabase->setText(settings.value("database").toString());
	settings.endGroup();
	settings.endGroup();

	settings.beginGroup("app");
	settings.beginGroup("logging");
	load_combo_text(ui->lstLogLevel, settings, "level");
	ui->edCardLog->setText(settings.value("cardLog").toString());
	settings.endGroup();
	settings.beginGroup("extensions");
	load_combo_data(ui->lstExtensionsCurrentName, settings, "currentName", false);
	settings.endGroup();
	settings.endGroup();
}

void DlgSettings::save()
{
	QSettings settings;
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

	settings.beginGroup("sql");
	settings.beginGroup("connection");
	settings.setValue("driver", ui->lstSqlDriver->itemData(ui->lstSqlDriver->currentIndex()));
	settings.setValue("host", ui->edSqlHost->text());
	settings.setValue("port", ui->edSqlPort->value());
	settings.setValue("user", ui->edSqlUser->text());
	settings.setValue("password", ui->edSqlPassword->text());
	settings.setValue("database", ui->edSqlDatabase->text());
	settings.endGroup();
	settings.endGroup();

	settings.beginGroup("app");
	settings.beginGroup("logging");
	settings.setValue("level", ui->lstLogLevel->currentText());
	settings.setValue("cardLog", ui->edCardLog->text());
	settings.endGroup();
	settings.beginGroup("extensions");
	settings.setValue("currentName", ui->lstExtensionsCurrentName->itemData(ui->lstExtensionsCurrentName->currentIndex()));
	settings.endGroup();
	settings.endGroup();
}

void DlgSettings::on_btCardLog_clicked()
{
	QString fn = QFileDialog::getSaveFileName(this, tr("Card log file"));
	if(!fn.isEmpty()) ui->edCardLog->setText(fn);
}

