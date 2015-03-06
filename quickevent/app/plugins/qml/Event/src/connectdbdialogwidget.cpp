#include "connectdbdialogwidget.h"
#include "ui_connectdbdialogwidget.h"

#include <qf/core/utils/settings.h>
#include <qf/core/utils/crypt.h>

namespace {
qf::core::utils::Crypt s_crypt(qf::core::utils::Crypt::createGenerator(16808, 11, 2147483647));
}

ConnectdbDialogWidget::ConnectdbDialogWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ConnectdbDialogWidget)
{
	setTitle("Connect to data ...");

	ui->setupUi(this);
}

ConnectdbDialogWidget::~ConnectdbDialogWidget()
{
	delete ui;
}

EventPlugin::ConnectionType ConnectdbDialogWidget::connectionType()
{
	if(ui->tabWidget->currentIndex() == 0)
		return EventPlugin::ConnectionType::SqlServer;
	return EventPlugin::ConnectionType::SingleFile;
}

QString ConnectdbDialogWidget::serverHost()
{
	return ui->edServerHost->text();
}

int ConnectdbDialogWidget::serverPort()
{
	return ui->edServerPort->value();
}

QString ConnectdbDialogWidget::serverUser()
{
	return ui->edServerUser->text();
}

QString ConnectdbDialogWidget::serverPassword()
{
	return ui->edServerPassword->text();
}

void ConnectdbDialogWidget::loadSettings()
{
	qf::core::utils::Settings settings;
	settings.beginGroup("event");
	ui->edEventName->setText(settings.value("eventName").toString());

	settings.beginGroup("dataStorage");
	int i = settings.value("connectionType", 0).toInt();
	ui->tabWidget->setCurrentIndex(i);

	settings.beginGroup("sqlServer");
	ui->edServerHost->setText(settings.value("host").toString());
	ui->edServerPort->setValue(settings.value("port").toInt());
	ui->edServerUser->setText(settings.value("user").toString());
	QByteArray ba = settings.value("password").toString().toUtf8();
	ui->edServerPassword->setText(s_crypt.decrypt(ba));
	settings.endGroup();

	settings.beginGroup("singleFile");
	ui->edSingleWorkingDir->setText(settings.value("workingDir").toString());
	settings.endGroup();

	settings.endGroup();
	settings.endGroup();
}

void ConnectdbDialogWidget::saveSettings()
{
	qf::core::utils::Settings settings;
	settings.beginGroup("event");

	settings.setValue("eventName", ui->edEventName->text());

	settings.beginGroup("dataStorage");
	settings.setValue("connectionType", ui->tabWidget->currentIndex());

	settings.beginGroup("sqlServer");
	settings.setValue("host", ui->edServerHost->text());
	settings.setValue("port", ui->edServerPort->value());
	settings.setValue("user", ui->edServerUser->text());
	QByteArray ba = s_crypt.encrypt(ui->edServerPassword->text(), 32);
	settings.setValue("password", QString::fromLatin1(ba));
	settings.endGroup();

	settings.beginGroup("singleFile");
	settings.setValue("workingDir", ui->edSingleWorkingDir->text());
	settings.endGroup();

	settings.endGroup();
	settings.endGroup();
}
