#include "emmaclient.h"
#include "emmaclientwidget.h"

#include "../eventplugin.h"

#include <quickevent/core/si/checkedcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>
#include <plugins/Runs/src/runsplugin.h>
#include <plugins/Relays/src/relaysplugin.h>

#include <QDir>
#include <QFile>
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

EmmaClient::EmmaClient(QObject *parent)
	: Super(EmmaClient::serviceName(), parent)
{
	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::dbEventNotify, this, &EmmaClient::onDbEventNotify, Qt::QueuedConnection);

	m_exportTimer = new QTimer(this);
	connect(m_exportTimer, &QTimer::timeout, this, &EmmaClient::onExportTimerTimeOut);
	connect(this, &EmmaClient::statusChanged, [this](Status status) {
		if(status == Status::Running) {
			if(settings().exportIntervalSec() > 0) {
				onExportTimerTimeOut();
				m_exportTimer->start();
			}
		}
		else {
			m_exportTimer->stop();
		}
	});
	connect(this, &EmmaClient::settingsChanged, this, &EmmaClient::init, Qt::QueuedConnection);

}

QString EmmaClient::serviceName()
{
	return QStringLiteral("EmmaClient");
}

void EmmaClient::exportRadioCodesRacomTxt()
{
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	if(!createExportDir()) {
		return;
	}
	QFile f_splitnames(export_dir + '/' + ss.fileNameBase() + ".splitnames.txt");
	if(!f_splitnames.open(QFile::WriteOnly)) {
		qfError() << "Canot open file:" << f_splitnames.fileName() << "for writing.";
		return;
	}
	qfInfo() << "EmmaClient: exporting code names to" << f_splitnames.fileName();
	QFile f_splitcodes(ss.exportDir() + '/' + ss.fileNameBase() + ".splitcodes.txt");
	if(!f_splitcodes.open(QFile::WriteOnly)) {
		qfError() << "Canot open file:" << f_splitcodes.fileName() << "for writing.";
		return;
	}
	qfInfo() << "EmmaClient: exporting codes to" << f_splitcodes.fileName();

	QTextStream ts_names(&f_splitnames);
	QTextStream ts_codes(&f_splitcodes);

	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();

	if (is_relays) {
		qfs::QueryBuilder qb_classes;
		qb_classes.select2("classes", "name")
				.select2("classdefs", "relayStartNumber, relayLegCount")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId=" + QString::number(current_stage))
				.orderBy("classes.name");
		qfs::Query q1;
		q1.execThrow(qb_classes.toString());
		while(q1.next()) {
			int relayStartNumber = q1.value("relayStartNumber").toInt();
			int relayLegCount = q1.value("relayLegCount").toInt();
			for (int leg = 1; leg <= relayLegCount; leg++)
			{
				qfs::QueryBuilder qb_codes;
				qb_codes.select2("codes", "*")
						.from("coursecodes, courses, codes")
						.where("coursecodes.codeId=codes.id")
						.where("coursecodes.courseId=courses.id")
						.where("courses.name=" + QString("'%1.%2'").arg(relayStartNumber).arg(leg))
						.where("codes.radio")
						.orderBy("coursecodes.position");
//				qfInfo() << qb_codes.toString();

				QString class_name = q1.value("classes.name").toString();
				class_name.remove(" ");
				class_name += QString("_%1").arg(leg);
				QVector<int> codes;
				qfs::Query q2;
				q2.execThrow(qb_codes.toString());
				while(q2.next()) {
					int code = q2.value("codes.code").toInt();
					codes << code;
				}

				ts_names << class_name;
				ts_codes << class_name;
				if(!codes.isEmpty()) {
					for(auto &code : codes) {
						ts_names << ' ' << QStringLiteral("cn%1").arg(code);
						ts_codes << ' ' << code;
					}
				}
				ts_names << " finish\n";
				ts_codes << ' ' << 2 << '\n';
			}
		}
	}
	else
	{
		qfs::QueryBuilder qb_classes;
		qb_classes.select2("classes", "name")
				.select2("classdefs", "courseId")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId=" + QString::number(current_stage))
				.orderBy("classes.name");
		qfs::Query q1;
		q1.execThrow(qb_classes.toString());
		while(q1.next()) {
			int course_id = q1.value("courseId").toInt();
			qfs::QueryBuilder qb_codes;
			qb_codes.select2("codes", "*")
					//.select2("coursecodes", "position")
					.from("coursecodes")
					.joinRestricted("coursecodes.codeId", "codes.id", "codes.radio", qfs::QueryBuilder::INNER_JOIN)
					.where("coursecodes.courseId=" + QString::number(course_id))
					.orderBy("coursecodes.position");
//			qfInfo() << qb_codes.toString();

			QString class_name = q1.value("classes.name").toString();
			class_name.remove(" ");
			QVector<int> codes;
			qfs::Query q2;
			q2.execThrow(qb_codes.toString());
			while(q2.next()) {
				int code = q2.value("codes.code").toInt();
				codes << code;
			}

			ts_names << class_name;
			ts_codes << class_name;
			if(!codes.isEmpty()) {
				for(auto &code : codes) {
					ts_names << ' ' << QStringLiteral("cn%1").arg(code);
					ts_codes << ' ' << code;
				}
			}
			ts_names << " finish\n";
			ts_codes << ' ' << 2 << '\n';
		}
	}
}

void EmmaClient::exportResultsIofXml3()
{
	if(!createExportDir())
		return;
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QString file_name = export_dir + '/' + ss.fileNameBase() + ".results.xml";
	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	if (is_relays) {
		QFile f(file_name);
		if(f.open(QFile::WriteOnly)) {
			f.write(getPlugin<RelaysPlugin>()->resultsIofXml30().toUtf8());
		}
	}
	else {
		getPlugin<RunsPlugin>()->exportResultsIofXml30Stage(current_stage, file_name);
	}
}

void EmmaClient::exportStartListIofXml3()
{
	if(!createExportDir())
		return;
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QString file_name = export_dir + '/' + ss.fileNameBase() + ".startlist.xml";
	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	if (is_relays) {
		QFile f(file_name);
		if(f.open(QFile::WriteOnly)) {
			f.write(getPlugin<RelaysPlugin>()->startListIofXml30().toUtf8());
		}
	}
	else {
		getPlugin<RunsPlugin>()->exportStartListStageIofXml30(current_stage, file_name);
	}
}

bool EmmaClient::createExportDir()
{
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QDir ed;
	if(!ed.mkpath(export_dir)) {
		qfError() << "Canot create export dir:" << export_dir;
		return false;
	}
	return true;
}

void EmmaClient::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	//qfInfo() << domain << data;
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED)) {
		onCardChecked(data.toMap());
	}
}

void EmmaClient::onCardChecked(const QVariantMap &data)
{
	Q_UNUSED(data)
/*  regenerate file every X second
	if(status() != Status::Running)
		return;
	quickevent::core::si::CheckedCard checked_card(data);
	QString s = QString("%1").arg(checked_card.cardNumber(), 8, 10, QChar(' '));
	s += QStringLiteral(": FIN/");
	int64_t msec = checked_card.stageStartTimeMs() + checked_card.finishTimeMs();
	QTime tm = QTime::fromMSecsSinceStartOfDay(msec);
	s += tm.toString(QStringLiteral("HH:mm:ss.zzz"));
	s += '0';
	s += '/';
	if (checked_card.finishTimeMs() > 0) {
		if (checked_card.isMisPunch() || checked_card.isBadCheck()) {
			s += QStringLiteral("MP  ");
		} else {
			//checked_card is OK
			s += QStringLiteral("O.K.");
		}
	} else {
		// DidNotFinish
		s += QStringLiteral("DNF ");
	}
	qfInfo() << "EmmaClient: " << s;
	EmmaClientSettings ss = settings();
	QString fn = ss.exportDir() + '/' + ss.fileName();
	QFile file(fn);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
		qfError() << "Cannot open file" << file.fileName() << "for writing, stopping service";
		stop();
		return;
	}
	QTextStream out(&file);
	out << s << "\n";*/
}

qf::qmlwidgets::framework::DialogWidget *EmmaClient::createDetailWidget()
{
	auto *w = new EmmaClientWidget();
	return w;
}

void EmmaClient::init()
{
	EmmaClientSettings ss = settings();
	if(ss.exportIntervalSec() > 0) {
		m_exportTimer->setInterval(ss.exportIntervalSec() * 1000);
	}
	else {
		m_exportTimer->stop();
	}
}

bool EmmaClient::preExport()
{
	EmmaClientSettings ss = settings();
	if(!QDir().mkpath(ss.exportDir())) {
		qfError() << "Cannot create export dir:" << ss.exportDir();
		return false;
	}
	return true;
}

void EmmaClient::onExportTimerTimeOut()
{
	if(status() != Status::Running)
		return;

	if (!preExport())
		return;

	EmmaClientSettings ss = settings();

	if (ss.exportStartListTypeXml3())
	{
		qfInfo() << "EmmaClient Start List Iof Xml3 creation called";
		exportStartListIofXml3();
	}
	if (ss.exportResultTypeXml3())
	{
		qfInfo() << "EmmaClient Result Iof Xml3 creation called";
		exportResultsIofXml3();
	}
	if (ss.exportStartTypeTxt())
	{
		qfInfo() << "EmmaClient startlist creation called";
		exportStartListRacomTxt();
	}
	if (ss.exportFinishTypeTxt())
	{
		qfInfo() << "EmmaClient finish creation called";
		exportFinishRacomTxt();
	}
}

void EmmaClient::exportFinishRacomTxt()
{
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QFile f(export_dir + '/' + ss.fileNameBase() + ".finish.txt");
	if(!f.open(QFile::WriteOnly)) {
		qfError() << "Canot open file:" << f.fileName() << "for writing.";
		return;
	}

	QTextStream ts(&f);

	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	int start00 = getPlugin<EventPlugin>()->stageStartMsec(current_stage);

	qfs::QueryBuilder qb;
	qfs::Query q2;

	qb.select2("runs", "siId, isRunning, finishTimeMs, misPunch, badCheck, disqualified, notCompeting")
			.select2("cards", "id")
			.from("runs")
			.join("runs.id","cards.runId")
			.where("runs.stageId=" QF_IARG(current_stage))
			.where("runs.siId IS NOT NULL")
			.orderBy("runs.finishTimeMs ASC ")
			.orderBy("runs.id ASC");
	q2.execThrow(qb.toString());
	int last_si = 0;
	while(q2.next()) {
		int si = q2.value("runs.siId").toInt();
		int fin_time = q2.value("runs.finishTimeMs").toInt();
		bool is_mis_punch = q2.value("runs.misPunch").toBool();
		bool is_bad_check = q2.value("runs.badCheck").toBool();
		bool is_disq = q2.value("runs.disqualified").toBool();
		bool is_running = q2.value("runs.isRunning").toBool();
		bool not_competing = q2.value("runs.notCompeting").toBool();
		int cards_id = q2.value("cards.id").toInt();

		if (si == 0 || last_si == si)
			continue; // without si or duplicate readout are unusable
		last_si = si;
		QString s = QString("%1").arg(si , 8, 10, QChar(' '));
		s += QStringLiteral(": FIN/");
		int msec = start00 + fin_time;
		QTime tm = QTime::fromMSecsSinceStartOfDay(msec);
		s += tm.toString(QStringLiteral("HH:mm:ss.zzz"));
		s += '0';
		s += '/';
		if (!is_running) {
			s += QStringLiteral("DNS ");
		} else if (fin_time > 0) {
			if (not_competing) {
				s += QStringLiteral("NC  ");
			} else if (is_disq) {
				if (is_mis_punch || is_bad_check)
					s += QStringLiteral("MP  ");
				else
					s += QStringLiteral("DISQ");
			} else {
				//checked_card is OK
				s += QStringLiteral("O.K.");
			}
		} else if (cards_id == 0) {
			continue; // skip if not yet readout
		} else {
			// DidNotFinish
			s += QStringLiteral("DNF ");
		}
		ts << s << "\n";
	}
}

void EmmaClient::exportStartListRacomTxt()
{
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QFile f(export_dir + '/' + ss.fileNameBase() + ".start.txt");
	if(!f.open(QFile::WriteOnly)) {
		qfError() << "Canot open file:" << f.fileName() << "for writing.";
		return;
	}

	QTextStream ts(&f);
	ts.setGenerateByteOrderMark(true); // BOM

	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	int current_stage = getPlugin<EventPlugin>()->currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("runs", "startTimeMs, siId, competitorId, isrunning, leg")
			.select2("competitors","firstName, lastName, registration")
			.select2("classes","name")
			.select2("cards", "id, startTime")
			.from("runs")
			.join("runs.competitorId","competitors.id")
			.join("runs.id", "cards.runId")
			.where("runs.stageId=" QF_IARG(current_stage));
	if(is_relays) {
		qb.select2("relays","number");
		qb.join("runs.relayId", "relays.id");
		qb.join("relays.classId", "classes.id");
		qb.orderBy("runs.leg, relays.number ASC");
	}
	else {
		qb.join("competitors.classId","classes.id");
		qb.orderBy("runs.id ASC");
	}

	int start00 = getPlugin<EventPlugin>()->stageStartMsec(current_stage);
	qfDebug() << qb.toString();
	qfs::Query q2;
	q2.execThrow(qb.toString());
	int last_id = -1;
	QMap <int,int> start_times_relays;
	while(q2.next()) {
		int id = q2.value("runs.competitorId").toInt();
		if (id == last_id)
			continue;
		bool is_running = (q2.value("runs.isrunning").isNull()) ? false : q2.value("runs.isrunning").toBool();
		if (!is_running)
			continue;
		last_id = id;
		int si = q2.value("runs.siId").toInt();
		int start_time = q2.value("runs.startTimeMs").toInt();
		bool start_time_card_null = q2.value("runs.startTimeMs").isNull();
		int start_time_card = q2.value("cards.startTime").toInt();
		if (start_time_card == 61166 || start_time_card_null)
			start_time_card = 0;
		QString name = q2.value("competitors.lastName").toString() + " " + q2.value("competitors.firstName").toString();
		QString class_name = q2.value("classes.name").toString();
		class_name.remove(" ");
		if (class_name.isEmpty())
			continue;
		QString reg = q2.value("competitors.registration").toString();
		name = name.leftJustified(22,QChar(' '),true);
		int id_or_bib = id;
		if (is_relays)
		{
			int leg = q2.value("runs.leg").toInt();
			class_name = QString("%1 %2").arg(class_name).arg(leg);
			class_name = class_name.leftJustified(7,QChar(' '),true);
			reg = reg.left(3);
			reg = reg.leftJustified(7,QChar(' '),true);
			// EmmaClient uses for all relays start time of 1st leg
			int rel_num =  q2.value("relays.number").toInt();
			if (leg == 1 && rel_num > 0) {
				start_times_relays.insert(rel_num,start_time);
			} else if (rel_num > 0) {
				auto it = start_times_relays.find(rel_num);
				if (it != start_times_relays.end())
					start_time = it.value();
			}
			id_or_bib = (rel_num > 0) ? rel_num : 10000+id; // when no number, use id with offset
		}
		else
		{
			class_name = class_name.leftJustified(7,QChar(' '),true);
			reg = reg.leftJustified(7,QChar(' '),true);
		}

		int msec = start_time;
		if (start_time_card != 0 && !start_time_card_null && !is_relays)
		{
			// has start in si card (P, T, HDR) & not used in relays
			start_time_card *= 1000; // msec
			start_time_card -= start00;
			if (start_time_card < 0) // 12h format
				start_time_card += (12*60*60*1000);
			msec = start_time_card;
		}

		QString tm2;
		//TODO zmenit na format mmm.ss,zzzz
		if (msec < 0)
			continue; // emma client has problem with negative times
		int min = (msec / 60000);
		if(min < 10)
			tm2 += "00";
		else if(min < 100)
			tm2 += "0";
		tm2 += QString::number(min);
		tm2 += '.';
		int sec = (msec % 60000 / 1000);
		if(sec < 10)
			tm2 += "0";
		tm2 += QString::number(sec);
		tm2 += ',';
		int zzzz = msec % 1000 * 10;
		if(zzzz < 10)
			tm2 += "000";
		else if(zzzz < 100)
			tm2 += "00";
		else if(zzzz < 1000)
			tm2 += "000";
		tm2 += QString::number(zzzz);

		if (id != 0) // filter bad data
		{
			QString s = QString("%1 %2 %3 %4 %5 %6").arg(id_or_bib , 5, 10, QChar(' ')).arg(si, 8, 10, QChar(' ')).arg(class_name).arg(reg).arg(name).arg(tm2);
			ts << s << "\n";
		}
	}
}

void EmmaClient::loadSettings()
{
	Super::loadSettings();
	init();
}

}}
