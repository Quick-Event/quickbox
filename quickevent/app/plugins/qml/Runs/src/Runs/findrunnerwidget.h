#ifndef RUNS_FINDRUNNERWIDGETWIDGET_H
#define RUNS_FINDRUNNERWIDGETWIDGET_H

#include "../runspluginglobal.h"

#include <QWidget>

namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}}
namespace qmlwidgets {
class TableView;
}
}

namespace Runs {

namespace Ui {
class FindRunnerWidget;
}

class RUNSPLUGIN_DECL_EXPORT FindRunnerWidget : public QWidget
{
	Q_OBJECT
public:
	enum class FocusWidget {Name, Registration};
public:
	explicit FindRunnerWidget(int stage_id, QWidget *parent = 0);
	~FindRunnerWidget() Q_DECL_OVERRIDE;

	void reload();

	void setFocusToWidget(FocusWidget fw);
	qf::qmlwidgets::TableView* tableView();
private:
	Ui::FindRunnerWidget *ui;
	qf::core::model::SqlTableModel *m_runnersModel;
	int m_stageId = 0;
};

}

#endif // RUNS_FINDRUNNERWIDGETWIDGET_H
