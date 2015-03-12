#ifndef COMPETITORSWIDGET_H
#define COMPETITORSWIDGET_H

#include <QFrame>

namespace Ui {
class CompetitorsWidget;
}
namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
}
class CompetitorsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CompetitorsWidget(QWidget *parent = 0);
	~CompetitorsWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
	Q_SLOT void onEventOpenChanged(bool open);
private:
	Ui::CompetitorsWidget *ui;
	qf::core::model::SqlTableModel *m_competitorsModel;
};

#endif // COMPETITORSWIDGET_H
