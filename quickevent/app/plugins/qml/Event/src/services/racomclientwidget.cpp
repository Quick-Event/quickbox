#include "racomclientwidget.h"
#include "ui_racomclientwidget.h"
#include "racomclient.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>

namespace services {

RacomClientWidget::RacomClientWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::RacomClientWidget)
{
	setPersistentSettingsId("RacomClientWidget");
	ui->setupUi(this);

	RacomClient *svc = service();
	if(svc) {
		RacomClientSettings ss = svc->settings();
		ui->edPort->setValue(ss.port());
	}
}

RacomClientWidget::~RacomClientWidget()
{
	delete ui;
}

bool RacomClientWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		/*
		if(!fn.isEmpty()) {
			QFile file(fn);
			if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
				qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("File '%1' cannot be open for writing.").arg(fn));
				return false;
			}
		}
		*/
		saveSettings();
	}
	return true;
}

RacomClient *RacomClientWidget::service()
{
	RacomClient *svc = qobject_cast<RacomClient*>(Service::serviceByName(RacomClient::serviceName()));
	QF_ASSERT(svc, RacomClient::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

void RacomClientWidget::saveSettings()
{
	RacomClient *svc = service();
	if(svc) {
		RacomClientSettings ss = svc->settings();
		ss.setPort(ui->edPort->value());
		svc->setSettings(ss);
	}
}

}
