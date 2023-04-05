#include "oresultsclientwidget.h"
#include "ui_oresultsclientwidget.h"
#include "oresultsclient.h"
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>

#include <plugins/Event/src/eventplugin.h>
using qf::qmlwidgets::framework::getPlugin;

namespace Event {
namespace services {

OResultsClientWidget::OResultsClientWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::OResultsClientWidget)
{
	setPersistentSettingsId("OResultsClientWidget");
	ui->setupUi(this);

	OResultsClient *svc = service();
	if(svc) {
		OResultsClientSettings ss = svc->settings();
		ui->edExportInterval->setValue(ss.exportIntervalSec());
		ui->edApiKey->setText(svc->apiKey());
	}

	connect(ui->btExportResultsXml30, &QPushButton::clicked, this, &OResultsClientWidget::onBtExportResultsXml30Clicked);
	connect(ui->btExportStartListXml30, &QPushButton::clicked, this, &OResultsClientWidget::onBtExportStartListXml30Clicked);
}

OResultsClientWidget::~OResultsClientWidget()
{
	delete ui;
}

bool OResultsClientWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		if(!saveSettings()) {
			return false;
		}
	}
	return true;
}

OResultsClient *OResultsClientWidget::service()
{
	OResultsClient *svc = qobject_cast<OResultsClient*>(Service::serviceByName(OResultsClient::serviceName()));
	QF_ASSERT(svc, OResultsClient::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

bool OResultsClientWidget::saveSettings()
{
	OResultsClient *svc = service();
	if(svc) {
		OResultsClientSettings ss = svc->settings();
		ss.setExportIntervalSec(ui->edExportInterval->value());
		svc->setApiKey(ui->edApiKey->text().trimmed());
		svc->setSettings(ss);
	}
	return true;
}

void OResultsClientWidget::onBtExportResultsXml30Clicked()
{
	OResultsClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportResultsIofXml3();
	}
}

void OResultsClientWidget::onBtExportStartListXml30Clicked()
{
	OResultsClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportStartListIofXml3();
	}
}
}}

