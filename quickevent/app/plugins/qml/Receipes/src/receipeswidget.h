#ifndef RECEIPESWIDGET_H
#define RECEIPESWIDGET_H

//#include <qf/core/log.h>

#include <QFrame>

class QPrinterInfo;

namespace Ui {
	class ReceipesWidget;
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

class ReceipesPartWidget;
namespace Event {
class EventPlugin;
}
namespace Receipes {
class ReceipesPlugin;
}

class ReceipesWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit ReceipesWidget(QWidget *parent = 0);
	~ReceipesWidget() Q_DECL_OVERRIDE;

	static const char *SETTINGS_PREFIX;

	void settleDownInPartWidget(ReceipesPartWidget *part_widget);

	Q_SLOT void reset() {reload();}
	Q_SLOT void reload();
private:
	Q_SLOT void on_btPrintNew_clicked();
	void onCustomContextMenuRequest(const QPoint &pos);
	void printSelectedCards();

	Receipes::ReceipesPlugin* receipesPlugin();
	Event::EventPlugin* eventPlugin();
	void onCardRead();
	void printNewCards();
	void loadNewCards();
	Q_SLOT void onDbEventNotify(const QString &domain, const QVariant &payload);

	bool printReceipe(int card_id);

	void createActions();
	void loadPrinters();
	QPrinterInfo currentPrinter();
	int currentStageId();
private:
	Ui::ReceipesWidget *ui;
	qf::core::model::SqlTableModel *m_cardsModel = nullptr;
};

#endif // RECEIPESWIDGET_H
