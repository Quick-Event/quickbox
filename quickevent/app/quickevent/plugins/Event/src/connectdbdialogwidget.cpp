#include "connectdbdialogwidget.h"
#include "ui_connectdbdialogwidget.h"
#include "connectionsettings.h"

#include <qf/qmlwidgets/dialogs/filedialog.h>
//#include <qf/qmlwidgets/framework/mainwindow.h>

ConnectDbDialogWidget::ConnectDbDialogWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ConnectDbDialogWidget)
{
	setPersistentSettingsId("ConnectDbDialogWidget");
	setTitle(tr("Data storage setup"));

	ui->setupUi(this);
}

ConnectDbDialogWidget::~ConnectDbDialogWidget()
{
	delete ui;
}

QString ConnectDbDialogWidget::eventName()
{
	return ui->edEventName->text();
}

Event::EventPlugin::ConnectionType ConnectDbDialogWidget::connectionType()
{
	if(ui->dataStorageTabWidget->currentIndex() == 0)
		return Event::EventPlugin::ConnectionType::SqlServer;
	return Event::EventPlugin::ConnectionType::SingleFile;
}

QString ConnectDbDialogWidget::serverHost()
{
	return ui->edServerHost->text();
}

int ConnectDbDialogWidget::serverPort()
{
	return ui->edServerPort->value();
}

QString ConnectDbDialogWidget::serverUser()
{
	return ui->edServerUser->text();
}

QString ConnectDbDialogWidget::serverPassword()
{
	return ui->edServerPassword->text();
}

QString ConnectDbDialogWidget::singleWorkingDir()
{
	return ui->edSingleWorkingDir->text();
}

void ConnectDbDialogWidget::loadSettings()
{
	ConnectionSettings settings;
	ui->edEventName->setText(settings.eventName());
	switch(settings.connectionType()) {
	case Event::EventPlugin::ConnectionType::SqlServer:
		ui->dataStorageTabWidget->setCurrentWidget(ui->tabPostgres);
		break;
	case Event::EventPlugin::ConnectionType::SingleFile:
		ui->dataStorageTabWidget->setCurrentWidget(ui->tabSqlite);
		break;
	}
	ui->edServerHost->setText(settings.serverHost());
	int port = settings.serverPort();
	if(port > 0)
		ui->edServerPort->setValue(port);
	ui->edServerUser->setText(settings.serverUser());
	ui->edServerPassword->setText(settings.serverPassword());
	ui->edSingleWorkingDir->setText(settings.singleWorkingDir());
}

void ConnectDbDialogWidget::saveSettings()
{
	ConnectionSettings settings;
	settings.setEventName(ui->edEventName->text());
	auto connection_type = Event::EventPlugin::ConnectionType::SqlServer;
	if(ui->dataStorageTabWidget->currentWidget() == ui->tabSqlite)
		connection_type = Event::EventPlugin::ConnectionType::SingleFile;
	settings.setConnectionType(connection_type);
	settings.setServerHost(ui->edServerHost->text());
	settings.setServerPort(ui->edServerPort->value());
	settings.setServerUser(ui->edServerUser->text());
	settings.setServerPassword(ui->edServerPassword->text());
	settings.setSingleWorkingDir(ui->edSingleWorkingDir->text());
}

void ConnectDbDialogWidget::on_btSingleWorkingDirectory_clicked()
{
	//qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	QString wd = qf::qmlwidgets::dialogs::FileDialog::getExistingDirectory(this, tr("Event files directory"), ui->edSingleWorkingDir->text());
	if(!wd.isEmpty())
		ui->edSingleWorkingDir->setText(wd);
}
