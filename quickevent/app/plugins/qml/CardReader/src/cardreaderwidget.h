#ifndef CARDREADERWIDGET_H
#define CARDREADERWIDGET_H

#include <qf/core/log.h>

#include <QFrame>

namespace Ui {
	class CardReaderWidget;
}

namespace qf {
namespace core { namespace model { class SqlTableModel; } }
namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
class Plugin;
}
}
}
namespace siut { class DeviceDriver; }

namespace quickevent { namespace audio { class Player; }}

namespace CardReader {
class CardReaderPlugin;
class CardChecker;
class ReadCard;
class CheckedCard;
}

namespace Event { class EventPlugin; }

class QTextStream;
class QFile;
class QComboBox;
class QCheckBox;

class SIMessageTransmitRecord;
class SIMessageData;
class SIMessageCardReadOut;

class CardReaderPartWidget;

class CardReaderWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CardReaderWidget(QWidget *parent = 0);
	~CardReaderWidget() Q_DECL_OVERRIDE;

	Q_SIGNAL void sendSICommand(int cmd, const QByteArray& data_params);
	Q_SIGNAL void logRequest(qf::core::Log::Level level, const QString &msg);
	void emitLogRequest(qf::core::Log::Level level, const QString &msg) {emit logRequest(level, msg);}

	void settleDownInPartWidget(CardReaderPartWidget *part_widget);

	Q_SLOT void reset();
	Q_SLOT void reload();

	Q_SLOT void onDbEventNotify(const QString &domain, const QVariant &payload);
private slots:
	void appendLog(qf::core::Log::Level level, const QString &msg);
	void processDriverInfo(qf::core::Log::Level level, const QString &msg);
	void processSIMessage(const SIMessageData &msg);
	void processDriverRawData(const QByteArray &data);
	void onCommOpen(bool checked);
private:
	void createActions();
	Q_SLOT void openSettings();
	//qf::core::Log::Level logLevelFromSettings();
	//QTextStream& cardLog();
	//void closeCardLog();
	siut::DeviceDriver *siDriver();

	void processSICard(const SIMessageCardReadOut &card);
	void processSIPunch(const SIMessageTransmitRecord &rec);

	void updateTableView(int card_id);

	CardReader::CardReaderPlugin* thisPlugin();
	qf::qmlwidgets::framework::Plugin* receiptsPlugin();
	Event::EventPlugin* eventPlugin();
	Q_SLOT void onCbxCardCheckersActivated(int ix);

	void onCustomContextMenuRequest(const QPoint &pos);
	void showSelectedCard();
	void assignRunnerToSelectedCard();

	quickevent::audio::Player* audioPlayer();
	void operatorAudioWakeUp();
	void operatorAudioNotify();
private:
	Ui::CardReaderWidget *ui;
	qf::qmlwidgets::Action *m_actCommOpen = nullptr;
	qf::qmlwidgets::Action *m_actSettings = nullptr;
	//QTextStream *m_cardLog = nullptr;
	//QFile *m_cardLogFile = nullptr;
	siut::DeviceDriver *f_siDriver = nullptr;
	qf::core::model::SqlTableModel *m_cardsModel = nullptr;
	QComboBox *m_cbxCardCheckers = nullptr;
	QCheckBox *m_cbxAutoRefresh = nullptr;
	quickevent::audio::Player *m_audioPlayer = nullptr;
};

#endif // CARDREADERWIDGET_H
