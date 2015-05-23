#ifndef COMPETITORWIDGET_H
#define COMPETITORWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

namespace quickevent {
namespace og {
class SqlTableModel;
} }

namespace Ui {
class CompetitorWidget;
}

class CompetitorWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	explicit CompetitorWidget(QWidget *parent = 0);
	~CompetitorWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void loadRunsTable();
	Q_SLOT void saveRunsTable();
private:
	Ui::CompetitorWidget *ui;
	quickevent::og::SqlTableModel *m_runsModel;
};

#endif // COMPETITORWIDGET_H
