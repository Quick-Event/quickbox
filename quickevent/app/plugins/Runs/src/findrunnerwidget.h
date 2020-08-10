#ifndef RUNS_FINDRUNNERWIDGETWIDGET_H
#define RUNS_FINDRUNNERWIDGETWIDGET_H

#include "runspluginglobal.h"

#include <QWidget>
#include <QVariantMap>

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
	static const QString UseSIInNextStages;
public:
	explicit FindRunnerWidget(int stage_id, QWidget *parent = 0);
	~FindRunnerWidget() Q_DECL_OVERRIDE;

	QVariantMap selectedRunner() const;

	void focusLineEdit();
private:
	Ui::FindRunnerWidget *ui;
};

}

#endif // RUNS_FINDRUNNERWIDGETWIDGET_H
