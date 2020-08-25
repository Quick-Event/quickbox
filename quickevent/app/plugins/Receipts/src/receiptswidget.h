#ifndef RECEIPTSWIDGET_H
#define RECEIPTSWIDGET_H

#include <QFrame>

namespace Ui {
	class ReceiptsWidget;
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

class ReceiptsPartWidget;
namespace Event {
class EventPlugin;
}
namespace Receipts {
class ReceiptsPlugin;
}

class ReceiptsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit ReceiptsWidget(QWidget *parent = 0);
	~ReceiptsWidget() Q_DECL_OVERRIDE;

	static const char *SETTINGS_PREFIX;

	void settleDownInPartWidget(ReceiptsPartWidget *part_widget);

	Q_SLOT void reset();
	Q_SLOT void reload();
	bool isAutoPrintEnabled();

private slots:
	void on_btPrinterOptions_clicked();

private:
	Q_SLOT void on_btPrintNew_clicked();
	void onCustomContextMenuRequest(const QPoint &pos);
	void printSelectedCards();

	Receipts::ReceiptsPlugin* receiptsPlugin();
	Event::EventPlugin* eventPlugin();
	void onCardRead();
	void printNewCards();
	void loadNewCards();
	Q_SLOT void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);

	bool printReceipt(int card_id);

	void lazyInit();
	void loadReceptList();
	void updateReceiptsPrinterLabel();

	void createActions();
	int currentStageId();
private:
	Ui::ReceiptsWidget *ui;
	qf::core::model::SqlTableModel *m_cardsModel = nullptr;
};

#endif // RECEIPTSWIDGET_H
