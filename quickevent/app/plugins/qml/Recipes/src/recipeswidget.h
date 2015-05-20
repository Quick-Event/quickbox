#ifndef RECIPESWIDGET_H
#define RECIPESWIDGET_H

//#include <qf/core/log.h>

#include <QFrame>

class QPrinterInfo;

namespace Ui {
	class RecipesWidget;
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

class RecipesPartWidget;
namespace Event {
class EventPlugin;
}
namespace Recipes {
class RecipesPlugin;
}

class RecipesWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit RecipesWidget(QWidget *parent = 0);
	~RecipesWidget() Q_DECL_OVERRIDE;

	static const char *SETTINGS_PREFIX;

	void settleDownInPartWidget(RecipesPartWidget *part_widget);

	Q_SLOT void reset() {reload();}
	Q_SLOT void reload();
private:
	Q_SLOT void on_btPrintNew_clicked();
	void onCustomContextMenuRequest(const QPoint &pos);
	void printSelectedCards();

	Recipes::RecipesPlugin* recipesPlugin();
	Event::EventPlugin* eventPlugin();
	void onCardRead();
	void printNewCards();
	void loadNewCards();
	Q_SLOT void onDbEventNotify(const QString &domain, const QVariant &payload);

	bool printRecipe(int card_id);

	void createActions();
	void loadPrinters();
	QPrinterInfo currentPrinter();
	int currentStageId();
private:
	Ui::RecipesWidget *ui;
	qf::core::model::SqlTableModel *m_cardsModel = nullptr;
};

#endif // RECIPESWIDGET_H
