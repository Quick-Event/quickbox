#include "webapiwidget.h"
#include "ui_webapiwidget.h"
#include "webapi.h"

#include <qf/core/assert.h>

#include <QDialog>

namespace CardReader {
namespace services {

WebApiWidget::WebApiWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::WebApiWidget)
{
	setPersistentSettingsId("WebApiWidget");
	ui->setupUi(this);

	WebApi *svc = service();
	if (svc) {
		WebApiSettings ss = svc->settings();
		ui->edTcpListenPort->setValue(ss.tcpListenPort());
	}
}

WebApiWidget::~WebApiWidget()
{
	delete ui;
}

bool WebApiWidget::acceptDialogDone(int result)
{
	if (result == QDialog::Accepted) {
		saveSettings();
	}
	return true;
}

WebApi *WebApiWidget::service()
{
	WebApi *svc = qobject_cast<WebApi*>(Event::services::Service::serviceByName(WebApi::serviceName()));
	QF_ASSERT(svc, WebApi::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

void WebApiWidget::saveSettings()
{
	WebApi *svc = service();
	if (svc) {
		WebApiSettings ss = svc->settings();
		ss.setTcpListenPort(ui->edTcpListenPort->value());
		svc->setSettings(ss);
	}
}

}}
