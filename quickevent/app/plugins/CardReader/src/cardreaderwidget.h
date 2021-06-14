#ifndef CARDREADERWIDGET_H
#define CARDREADERWIDGET_H

#include <siut/sitask.h>

#include <qf/core/exception.h>
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

namespace siut { class DeviceDriver; class CommPort; class SICard; class SIPunch; }

namespace quickevent { namespace gui { namespace audio { class Player; }}}
namespace quickevent { namespace core { namespace si { class ReadCard; class CheckedCard; }}}

namespace CardReader {
class CardReaderPlugin;
class CardChecker;
}

namespace Event { class EventPlugin; }

class QTextStream;
class QFile;
class QComboBox;
class QCheckBox;
class QLabel;

class SIMessageTransmitPunch;
class SIMessageData;
class SIMessageCardReadOut;

class CardReaderPartWidget;

class CardReaderWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
	enum PunchMode { Readout, Edit_on_punch };
public:
	explicit CardReaderWidget(QWidget *parent = 0);
	~CardReaderWidget() Q_DECL_OVERRIDE;

	//Q_SIGNAL void sendSICommand(int cmd, const QByteArray& data_params);
	Q_SIGNAL void logRequest(NecroLog::Level level, const QString &msg);
	void emitLogRequest(NecroLog::Level level, const QString &msg) {emit logRequest(level, msg);}

	void settleDownInPartWidget(CardReaderPartWidget *part_widget);

	Q_SLOT void reset();
	Q_SLOT void reload();

	//Q_SLOT void processSIMessage(const SIMessageData &msg);

	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
private slots:
	void appendLog(NecroLog::Level level, const QString &msg);
	void processDriverInfo(NecroLog::Level level, const QString &msg);
	void processDriverRawData(const QByteArray &data);
	void onOpenCommTriggered(bool checked);

	void onSiTaskFinished(int task_type, QVariant result);

	void readStationBackupMemory();

	void importCards_lapsOnlyCsv();
	void importCards_SIReaderBackupMemoryCsv();
private:
	void createActions();
	Q_SLOT void openSettings();

	siut::DeviceDriver *siDriver();
	siut::CommPort *commPort();
	void onComOpenChanged(bool comm_is_open);

	void processSICard(const siut::SICard &card);
	void processSIPunch(const siut::SIPunch &rec);

	bool processReadCardInTransaction(const quickevent::core::si::ReadCard &read_card);
	void processReadCard(const quickevent::core::si::ReadCard &read_card);

	void updateTableView(int card_id);

	Q_SLOT void onCbxCardCheckersActivated(int ix);

	void onCustomContextMenuRequest(const QPoint &pos);
	void showSelectedReceipt();
	void showSelectedCard();
	void assignRunnerToSelectedCard();

	quickevent::gui::audio::Player* audioPlayer();
	void operatorAudioWakeUp();
	void operatorAudioNotify();
	int currentPunchMode();
private:
	Ui::CardReaderWidget *ui;
	QLabel *m_lblCommInfo = nullptr;
	qf::qmlwidgets::Action *m_actCommOpen = nullptr;
	qf::qmlwidgets::Action *m_actSettings = nullptr;
	qf::core::model::SqlTableModel *m_cardsModel = nullptr;
	QComboBox *m_cbxCardCheckers = nullptr;
	QComboBox *m_cbxPunchMode = nullptr;
	quickevent::gui::audio::Player *m_audioPlayer = nullptr;
	siut::DeviceDriver *f_siDriver = nullptr;
	siut::CommPort *m_commPort = nullptr;
};

#endif // CARDREADERWIDGET_H
