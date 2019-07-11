#include "emmaclient.h"
#include "emmaclientwidget.h"

#include "../Event/eventplugin.h"

#include <quickevent/core/si/checkedcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
//namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace services {

//static auto SETTING_KEY_FILE_NAME = QStringLiteral("fileName");

EmmaClient::EmmaClient(QObject *parent)
	: Super(EmmaClient::serviceName(), parent)
{
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &EmmaClient::onDbEventNotify, Qt::QueuedConnection);
	connect(this, &EmmaClient::statusChanged, [this]() {
		if(status() == Status::Running) {
			if(!m_exportResultsTimer) {
				m_exportResultsTimer = new QTimer(this);
				m_exportResultsTimer->setInterval(5*60*1000);
				connect(m_exportResultsTimer, &QTimer::timeout, this, &EmmaClient::exportResultsIofXml3);
			}
			m_exportResultsTimer->start();
		}
		else if(status() == Status::Stopped) {
			if(m_exportResultsTimer)
				m_exportResultsTimer->stop();
		}
	});
}

QString EmmaClient::serviceName()
{
	return QStringLiteral("EmmaClient");
}

void EmmaClient::exportRadioCodes()
{
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QDir ed;
	if(!createExportDir()) {
		return;
	}
	QString event_name = eventPlugin()->eventName();
	QFile f_splitnames(export_dir + '/' + event_name + ".splitnames.txt");
	if(!f_splitnames.open(QFile::WriteOnly)) {
		qfError() << "Canot open file:" << f_splitnames.fileName() << "for writing.";
		return;
	}
	qfInfo() << "EmmaClient: exporting code names to" << f_splitnames.fileName();
	QFile f_splitcodes(ss.exportDir() + '/' + event_name + ".splitcodes.txt");
	if(!f_splitcodes.open(QFile::WriteOnly)) {
		qfError() << "Canot open file:" << f_splitcodes.fileName() << "for writing.";
		return;
	}
	qfInfo() << "EmmaClient: exporting codes to" << f_splitcodes.fileName();

	QTextStream ts_names(&f_splitnames);
	QTextStream ts_codes(&f_splitcodes);

	int current_stage = eventPlugin()->currentStageId();
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
		//qfInfo() << qb_codes.toString();

		QString class_name = q1.value("classes.name").toString();
		QVector<int> codes;
		qfs::Query q2;
		q2.execThrow(qb_codes.toString());
		while(q2.next()) {
			int code = q2.value("codes.code").toInt();
			codes << code;
		}
		if(!codes.isEmpty()) {
			ts_names << class_name;
			ts_codes << class_name;
			for(int code : codes) {
				ts_names << ' ' << QStringLiteral("cn%1").arg(code);
				ts_codes << ' ' << code;
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
	QString event_name = eventPlugin()->eventName();
	EmmaClientSettings ss = settings();
	QString export_dir = ss.exportDir();
	QString file_name = export_dir + '/' + event_name + ".results.xml";
	int current_stage = eventPlugin()->currentStageId();
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	QObject *plugin = fwk->plugin("Runs");
	QMetaObject::invokeMethod(plugin, "exportResultsIofXml30Stage",
							  Q_ARG(int, current_stage),
							  Q_ARG(QString, file_name) );
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
	out << s << "\n";
}

qf::qmlwidgets::framework::DialogWidget *EmmaClient::createDetailWidget()
{
	auto *w = new EmmaClientWidget();
	return w;
}

} // namespace services
