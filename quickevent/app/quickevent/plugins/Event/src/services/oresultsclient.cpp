#include "oresultsclient.h"
#include "oresultsclientwidget.h"

#include "../eventplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <plugins/Runs/src/runsplugin.h>
#include <plugins/Relays/src/relaysplugin.h>

#include <QDir>
#include <QFile>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;
using Relays::RelaysPlugin;
using Runs::RunsPlugin;

namespace Event {
namespace services {

OResultsClient::OResultsClient(QObject *parent)
	: Super(OResultsClient::serviceName(), parent)
{
	m_networkManager = new QNetworkAccessManager(this);
	m_exportTimer = new QTimer(this);
	connect(m_exportTimer, &QTimer::timeout, this, &OResultsClient::onExportTimerTimeOut);
	connect(this, &OResultsClient::statusChanged, [this](Status status) {
		if(status == Status::Running) {
			onExportTimerTimeOut();
			m_exportTimer->start();
		}
		else {
			m_exportTimer->stop();
		}
	});
	connect(this, &OResultsClient::settingsChanged, this, &OResultsClient::init, Qt::QueuedConnection);

}

QString OResultsClient::serviceName()
{
	return QStringLiteral("OResults");
}

void OResultsClient::exportResultsIofXml3()
{
	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();

	QString str = is_relays
			? getPlugin<RelaysPlugin>()->resultsIofXml30()
			: getPlugin<RunsPlugin>()->resultsIofXml30Stage(current_stage);

	sendFile("results upload", "/results", str);
}

void OResultsClient::exportStartListIofXml3()
{

	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();

	QString str = is_relays
			? getPlugin<RelaysPlugin>()->startListIofXml30()
			: getPlugin<RunsPlugin>()->startListStageIofXml30(current_stage);

	sendFile("start list upload", "/start-lists?format=xml", str);
}

qf::qmlwidgets::framework::DialogWidget *OResultsClient::createDetailWidget()
{
	auto *w = new OResultsClientWidget();
	return w;
}

void OResultsClient::init()
{
	OResultsClientSettings ss = settings();
	m_exportTimer->setInterval(ss.exportIntervalSec() * 1000);
}

void OResultsClient::onExportTimerTimeOut()
{
	if(status() != Status::Running)
		return;

	exportResultsIofXml3();
}

void OResultsClient::loadSettings()
{
	Super::loadSettings();
	init();
}

void OResultsClient::sendFile(QString name, QString request_path, QString file) {

	QHttpMultiPart *multi_part = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart api_key_part;
	auto api_key = settings().apiKey();
	api_key_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"apiKey\""));
	api_key_part.setBody(api_key.toUtf8());

	QHttpPart file_part;
	file_part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/xml"));
	file_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\""));
	file_part.setBody(file.toUtf8());

	multi_part->append(api_key_part);
	multi_part->append(file_part);

	QUrl url(API_URL + request_path);
	QNetworkRequest request(url);
	QNetworkReply *reply = m_networkManager->post(request, multi_part);

	connect(reply, &QNetworkReply::finished, [reply, name]()
	{
		if(reply->error())
		{
			qfError() << "OReuslts.eu [" + name + "]: " + QString(reply->readAll());
		}
		else
		{
			qfInfo() << "OReuslts.eu [" + name + "]: success";
		}
		reply->deleteLater();
	});
}

}}
