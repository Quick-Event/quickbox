#include "shvclientservicewidget.h"
#include "ui_shvclientservicewidget.h"
#include "shvclientservice.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/core/assert.h>

#include <QFileDialog>
#include <QUrlQuery>
#include <QClipboard>

#include <plugins/Event/src/eventplugin.h>
using qf::qmlwidgets::framework::getPlugin;

namespace Event::services::shvapi {

ShvClientServiceWidget::ShvClientServiceWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::ShvClientServiceWidget)
{
	setPersistentSettingsId("ShvClientServiceWidget");
	ui->setupUi(this);

	auto *svc = service();
	if(svc) {
		auto ss = svc->settings();
		ui->shvUrl->setText(ss.shvConnectionUrl());
		ui->shvEventPath->setText(ss.eventPath());
		ui->shvApiKey->setText(ss.apiKey());
	}
	updateStarterToolUrl();
	connect(ui->shvUrl, &QLineEdit::textChanged, this, &ShvClientServiceWidget::updateStarterToolUrl);
	connect(ui->shvApiKey, &QLineEdit::textChanged, this, &ShvClientServiceWidget::updateStarterToolUrl);
	connect(ui->shvEventPath, &QLineEdit::textChanged, this, &ShvClientServiceWidget::updateStarterToolUrl);
	connect(ui->btGenerateApiKey, &QAbstractButton::clicked, this, [this]() {
		auto *event_plugin = getPlugin<EventPlugin>();
		ui->shvApiKey->setText(event_plugin->createShvApiKey());
	});
	connect(ui->btCopyUrl, &QAbstractButton::clicked, this, [this]() {
		QClipboard *clipboard = QGuiApplication::clipboard();
		clipboard->setText(ui->qrCodeUrl->text());
	});
}

ShvClientServiceWidget::~ShvClientServiceWidget()
{
	delete ui;
}

bool ShvClientServiceWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		if(!saveSettings()) {
			return false;
		}
	}
	return true;
}

ShvClientService *ShvClientServiceWidget::service()
{
	ShvClientService *svc = qobject_cast<ShvClientService*>(Service::serviceByName(ShvClientService::serviceName()));
	QF_ASSERT(svc, ShvClientService::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

bool ShvClientServiceWidget::saveSettings()
{
	auto *svc = service();
	if(svc) {
		auto ss = svc->settings();
		ss.setShvConnectionUrl(ui->shvUrl->text());
		ss.setApiKey(ui->shvApiKey->text().trimmed());
		svc->setSettings(ss);
	}
	return true;
}

void ShvClientServiceWidget::updateStarterToolUrl()
{
	QUrl url(ui->shvUrl->text());
	url.setScheme("tcp");
	url.setPath("/" + ui->shvEventPath->text());
	auto query = QUrlQuery(url);
	url.setQuery(QString());
	query.addQueryItem("api_key", ui->shvApiKey->text());
	url.setQuery(query);
	auto s = url.toString();
	ui->qrCodeUrl->setText(s);
}

}

