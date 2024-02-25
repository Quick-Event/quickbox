#include "clientwidget.h"
#include "ui_clientwidget.h"
#include "client.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/core/assert.h>

#include <QFileDialog>

#include <plugins/Event/src/eventplugin.h>
using qf::qmlwidgets::framework::getPlugin;

namespace Event::services::shvapi {

ClientWidget::ClientWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::ClientWidget)
{
	setPersistentSettingsId("ClientWidget");
	ui->setupUi(this);

	Client *svc = service();
	if(svc) {
		ClientSettings ss = svc->settings();
		ui->shvUrl->setText(ss.shvConnectionUrl());
	}
}

ClientWidget::~ClientWidget()
{
	delete ui;
}

bool ClientWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		if(!saveSettings()) {
			return false;
		}
	}
	return true;
}

Client *ClientWidget::service()
{
	Client *svc = qobject_cast<Client*>(Service::serviceByName(Client::serviceName()));
	QF_ASSERT(svc, Client::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

bool ClientWidget::saveSettings()
{
	Client *svc = service();
	if(svc) {
		ClientSettings ss = svc->settings();
		ss.setShvConnectionUrl(ui->shvUrl->text());
		//svc->setApiKey(ui->edApiKey->text().trimmed());
		svc->setSettings(ss);
	}
	return true;
}

}

