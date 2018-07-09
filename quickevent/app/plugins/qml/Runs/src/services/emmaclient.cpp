#include "emmaclient.h"
#include "emmaclientwidget.h"

#include "../Event/eventplugin.h"

#include <quickevent/core/si/checkedcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QFile>
#include <QSettings>
#include <QStandardPaths>

namespace services {

//const char *EmmaClient::SETTINGS_GROUP = "services/EmmaClient";
const char *ResultsExporter::SETTING_KEY_FILE_NAME = "fileName";

ResultsExporter::ResultsExporter(QObject *parent)
	: Super(ResultsExporter::serviceName(), parent)
{
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &ResultsExporter::onDbEventNotify, Qt::QueuedConnection);
}

QString ResultsExporter::serviceName()
{
	return QStringLiteral("EmmaClient");
}

void ResultsExporter::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_CHECKED)) {
		onCardChecked(data.toMap());
	}
}

void ResultsExporter::onCardChecked(const QVariantMap &data)
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
		if (checked_card.isMisPunch()) {
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
	QFile file(m_fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
		qfError() << "Cannot open file" << file.fileName() << "for writing, stopping service";
		stop();
		return;
	}
	QTextStream out(&file);
	out << s << "\n";
}

qf::qmlwidgets::framework::DialogWidget *ResultsExporter::createDetailWidget()
{
	auto *w = new ResultsExporterWidget();
	return w;
}

void ResultsExporter::loadSettings()
{
	QSettings settings;
	settings.beginGroup(settingsGroup());
	m_fileName = settings.value(ResultsExporter::SETTING_KEY_FILE_NAME).toString();
	if(m_fileName.isEmpty()) {
		m_fileName = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/FIN_LIVE.txt";
	}
	qfInfo() << "EmmaClient file name:" << fileName();
}

} // namespace services
