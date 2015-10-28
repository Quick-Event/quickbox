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
	setTitle("Data storage setup");

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
	if(ui->tabWidget->currentIndex() == 0)
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

void ConnectDbDialogWidget::loadSettings()
{
	ConnectionSettings settings;
	ui->edEventName->setText(settings.eventName());
	ui->tabWidget->setCurrentIndex(static_cast<int>(settings.connectionType()));
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
	settings.setConnectionType(static_cast<Event::EventPlugin::ConnectionType>(ui->tabWidget->currentIndex()));
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
