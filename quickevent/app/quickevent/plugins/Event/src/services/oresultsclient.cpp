#include "oresultsclient.h"
#include "oresultsclientwidget.h"

#include "../eventplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <plugins/Runs/src/runsplugin.h>
#include <plugins/Relays/src/relaysplugin.h>
#include <quickevent/core/si/checkedcard.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/sql/query.h>

#include <QDir>
#include <QFile>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <regex>

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
	connect(this, &OResultsClient::settingsChanged, this, &OResultsClient::init, Qt::QueuedConnection);
	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::dbEventNotify, this, &OResultsClient::onDbEventNotify, Qt::QueuedConnection);
}

QString OResultsClient::serviceName()
{
	return QStringLiteral("OResults");
}

void OResultsClient::run() {
	Super::run();
	onExportTimerTimeOut();
	m_exportTimer->start();
}

void OResultsClient::stop() {
	Super::stop();
	m_exportTimer->stop();
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

	sendFile("start list upload", "/start-lists", str);
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
	file_part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/gzip"));
	file_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\""));
	file_part.setBody(gzipCompress(file.toUtf8()));

	multi_part->append(api_key_part);
	multi_part->append(file_part);

	QUrl url(API_URL + request_path);
	QNetworkRequest request(url);
	QNetworkReply *reply = m_networkManager->post(request, multi_part);

	connect(reply, &QNetworkReply::finished, reply, [reply, name]()
	{
		if(reply->error()) {
			auto err_msg = "OReuslts.eu [" + name + "]: ";
			auto response_body = reply->readAll();
			if (!response_body.isEmpty())
				err_msg += response_body + " | ";
			qfError() << err_msg + reply->errorString();
		}
		else {
			qfInfo() << "OReuslts.eu [" + name + "]: success";
		}
		reply->deleteLater();
	});
}

void OResultsClient::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	if (status() != Status::Running)
		return;
	Q_UNUSED(connection_id)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED)) {
		auto checked_card = quickevent::core::si::CheckedCard(data.toMap());
		int competitor_id = getPlugin<RunsPlugin>()->competitorForRun(checked_card.runId());
		onCompetitorChanged(competitor_id);
	}
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_COMPETITOR_EDITED)) {
		int competitor_id = data.toInt();
		onCompetitorChanged(competitor_id);
	}
}

void OResultsClient::sendCompetitorChange(QString xml) {
	QUrl url(API_URL + "/meos");
	QNetworkRequest request(url);
	request.setRawHeader("pwd", settings().apiKey().toUtf8());
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/gzip" );
	QNetworkReply *reply = m_networkManager->post(request, gzipCompress(xml.toUtf8()));

	connect(reply, &QNetworkReply::finished, reply, [reply]()
	{
		if(reply->error()) {
			qfError() << "OReuslts.eu [competitor change]:" << reply->errorString();
		}
		else {
			QString msg = reply->readAll();
			if (msg.contains("BADPWD"))
				qfError() << "OReuslts.eu [competitor change]: Invalid API key";
			else if (!msg.contains("OK"))
				qfError() << "OReuslts.eu [competitor change]: Failed to process request";
		}
		reply->deleteLater();
	});
}

static void append_list(QVariantList &lst, const QVariantList &new_lst)
{
	lst.insert(lst.count(), new_lst);
}

static bool is_csos_reg(QString &reg)
{
	const static std::regex csos_registration_regex("[A-Z]{3}[0-9]{4}");
	return reg.length() == 7 && std::regex_match(reg.toStdString(), csos_registration_regex);
}

static int mop_start(int runner_start_ms) {
	int stage_id = getPlugin<RunsPlugin>()->selectedStageId();
	QDateTime event_start = getPlugin<EventPlugin>()->stageStartDateTime(stage_id);
	QDateTime runner_start = event_start.addMSecs(runner_start_ms);
	return event_start.date().startOfDay().msecsTo(runner_start) / 100;
}

static int mop_run_status_code(
		int time,
		bool isDisq,
		bool isDisqByOrganizer,
		bool isMissPunch,
		bool isBadCheck,
		bool isDidNotStart,
		bool isDidNotFinish,
		bool isNotCompeting)
{
	if (isNotCompeting)
		return 99; // NP (Not participating)
	if (isMissPunch)
		return 3; //MP (Missing punch)
	if (isDidNotFinish)
		return 4; // DNF (Did not finish)
	if (isDidNotStart)
		return 20; // DNS (Did not start)
	if (isBadCheck || isDisqByOrganizer || isDisq)
		return 5; // DQ (Disqualified)
	if (time)
		return 1; // OK
	return 0; // Unknown
}


void OResultsClient::onCompetitorChanged(int competitor_id)
{
	if (competitor_id == 0)
		return;

	int stage_id = getPlugin<RunsPlugin>()->selectedStageId();
	qf::core::sql::Query q;
	q.exec("SELECT competitors.registration, "
		   "competitors.startNumber, "
		   "competitors.lastName || ' ' || competitors.firstName AS name, "
		   "classes.id AS classId, "
		   "runs.siId, "
		   "runs.disqualified, "
		   "runs.disqualifiedByOrganizer, "
		   "runs.misPunch, "
		   "runs.badCheck, "
		   "runs.notStart, "
		   "runs.notFinish, "
		   "runs.notCompeting, "
		   "runs.startTimeMs, "
		   "runs.timeMs "
		   "FROM runs "
		   "INNER JOIN competitors ON competitors.id = runs.competitorId "
		   "LEFT JOIN relays ON relays.id = runs.relayId  "
		   "INNER JOIN classes ON classes.id = competitors.classId OR classes.id = relays.classId  "
		   "WHERE competitors.id=" QF_IARG(competitor_id) " AND runs.stageId=" QF_IARG(stage_id), qf::core::Exception::Throw);
	if(q.next()) {
		QString registration = q.value("registration").toString();
		int start_num = q.value("startNumber").toInt();
		QString name = q.value("name").toString();
		QString class_id = q.value("classId").toString();
		int card_num = q.value("siId").toInt();
		bool isDisq = q.value("disqualified").toBool();
		bool isDisqByOrganizer = q.value("disqualifiedByOrganizer").toBool();
		bool isMissPunch = q.value("misPunch").toBool();
		bool isBadCheck = q.value("badCheck").toBool();
		bool isDidNotStart = q.value("notStart").toBool();
		bool isDidNotFinish = q.value("notFinish").toBool();
		bool isNotCompeting = q.value("notCompeting").toBool();
		int start_time = q.value("startTimeMs").toInt();
		int running_time = q.value("timeMs").toInt();

		QString runner_id = is_csos_reg(registration) ? registration : QString::number(card_num);
		int status_code = mop_run_status_code(running_time, isDisq, isDisqByOrganizer, isMissPunch, isBadCheck, isDidNotStart, isDidNotFinish, isNotCompeting);

		if (runner_id.isEmpty() || card_num == 0)
			return;

		QVariantMap competitor {
			{"stat", status_code},
			{"cls", class_id}
		};
		if (start_num != 0)
			competitor.insert("bib", start_num);
		if(start_time != 0)
			competitor.insert("st", mop_start(start_time));
		if(running_time != 0)
			competitor.insert("rt", running_time / 100);


		QVariantList xml_root{"MOPDiff",
			QVariantMap {
				{"xmlns", "http://www.melin.nu/mop"},
				{"creator", "QuickEvent"},
				{"createTime", QDateTime::currentDateTimeUtc().toString(Qt::ISODate)}
			}
		};
		QVariantList xml_competitor{"cmp",
			QVariantMap {
				{"id", runner_id },
				{"card", card_num },
			},
			QVariantList {"base",
				competitor,
				name
			}
		};
		append_list(xml_root, xml_competitor);
		qf::core::utils::HtmlUtils::FromXmlListOptions opts;
		opts.setDocumentTitle("Competitor change");
		auto xml_paylaod = qf::core::utils::HtmlUtils::fromXmlList(xml_root, opts);
		sendCompetitorChange(xml_paylaod);
	}
}

QByteArray OResultsClient::gzipCompress(QByteArray data)
{
	// calc crc32 helper func
	auto crc32 = [](char *buf, uint32_t len){
		uint32_t val, crc;
		uint8_t i;

		crc = 0xFFFFFFFF;
		while(len--){
			val=(crc^*buf++)&0xFF;
			for(i=0; i<8; i++){
				val = val & 1 ? (val>>1)^0xEDB88320 : val>>1;
			}
			crc = val^crc>>8;
		}
		return crc^0xFFFFFFFF;
	};

	QByteArray compressedData = qCompress(data);
	//  Strip the first six bytes (a 4-byte length put on by qCompress and a 2-byte zlib header)
	// and the last four bytes (a zlib integrity check).
	compressedData.remove(0, 6);
	compressedData.chop(4);

	QByteArray header;
	QDataStream ds1(&header, QIODevice::WriteOnly);
	// Prepend a generic 10-byte gzip header (see RFC 1952),
	ds1 << quint16(0x1f8b)
		<< quint16(0x0800)
		<< quint16(0x0000)
		<< quint16(0x0000)
		<< quint16(0x000b);


	// Append a four-byte CRC-32 of the uncompressed data
	// Append 4 bytes uncompressed input size modulo 2^32
	QByteArray footer;
	QDataStream ds2(&footer, QIODevice::WriteOnly);
	ds2.setByteOrder(QDataStream::LittleEndian);
	ds2 << crc32(data.data(), data.length())
		<< quint32(data.size());

	return header + compressedData + footer;
}

}}
