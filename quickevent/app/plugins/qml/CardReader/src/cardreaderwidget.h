#ifndef CARDREADERWIDGET_H
#define CARDREADERWIDGET_H

#include <qf/core/log.h>

#include <QFrame>

namespace Ui {
	class CardReaderWidget;
}

namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
}
}
}
namespace siut {
class DeviceDriver;
}

class SIMessageData;
class SIMessageCardReadOut;

class QTextStream;
class QFile;

class CardReaderPartWidget;

class CardReaderWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CardReaderWidget(QWidget *parent = 0);
	~CardReaderWidget() Q_DECL_OVERRIDE;

	static const char *SETTINGS_PREFIX;

	void settleDownInPartWidget(qf::qmlwidgets::framework::PartWidget *part_widget);

	Q_SIGNAL void sendSICommand(int cmd, const QByteArray& data_params);
	Q_SIGNAL void logRequest(int level, const QString &msg);
	void emitLogRequest(int level, const QString &msg) {emit logRequest(level, msg);}

	void settleDownInPartWidget(CardReaderPartWidget *part_widget);
	Q_SLOT void reset() {reload();}
	Q_SLOT void reload();
private slots:
	void appendLog(int level, const QString &msg);
	void appendLogPre(int level, const QString &msg);
	void processDriverInfo(int level, const QString &msg);
	void processSIMessage(const SIMessageData &msg);
	void processSICard(const SIMessageCardReadOut &card);
	void processDriverRawData(const QByteArray &data);
	void onCommOpen(bool checked);
private:
	void createActions();
	Q_SLOT void openSettings();
	qf::core::Log::Level logLevelFromSettings();
	QTextStream& cardLog();
	void closeCardLog();
	siut::DeviceDriver *siDriver();

	int currentStage();

	int findRunId(const SIMessageCardReadOut &card);
	int saveCardToSql(const SIMessageCardReadOut &card, int run_id);
	int updateRunLapsSql(const SIMessageCardReadOut &card, int run_id);
	void updateTableView(int card_id);
private:
	Ui::CardReaderWidget *ui;
	qf::qmlwidgets::Action *m_actCommOpen = nullptr;
	qf::qmlwidgets::Action *m_actSettings = nullptr;
	QTextStream *m_cardLog = nullptr;
	QFile *m_cardLogFile = nullptr;
	siut::DeviceDriver *f_siDriver = nullptr;
	qf::core::model::SqlTableModel *m_cardsModel = nullptr;
};

#endif // CARDREADERWIDGET_H
