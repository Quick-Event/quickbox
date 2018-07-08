#include "emmaclient.h"
#include "../Event/eventplugin.h"

#include <quickevent/core/si/checkedcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QFile>

namespace services {

Event::EventPlugin *eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

EmmaClient::EmmaClient(QObject *parent)
	: Super(parent)
{
	setObjectName("EmmaClient");
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &EmmaClient::onDbEventNotify, Qt::QueuedConnection);
}

void EmmaClient::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_CHECKED)) {
		onCardChecked(data.toMap());
	}

}

void EmmaClient::onCardChecked(const QVariantMap &data)
{
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
	QFile file("FIN_LIVE.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream out(&file);
	out << s << "\n";
	file.close();
}

} // namespace services
