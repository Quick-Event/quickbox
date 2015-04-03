#ifndef RECEIPESWIDGET_H
#define RECEIPESWIDGET_H

//#include <qf/core/log.h>

#include <QFrame>

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

namespace CardReader {
//class CardChecker;
//class CheckedCard;
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
	void createActions();
	int currentStageId();
private:
	Ui::ReceipesWidget *ui;
	qf::core::model::SqlTableModel *m_printJobsModel = nullptr;
};

#endif // RECEIPESWIDGET_H
