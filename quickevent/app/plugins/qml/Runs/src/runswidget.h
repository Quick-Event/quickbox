#ifndef RUNSWIDGET_H
#define RUNSWIDGET_H

#include <QFrame>

namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
namespace qmlwidgets {
class ForeignKeyComboBox;
}
}

namespace Event {
class EventPlugin;
}

namespace Ui {
class RunsWidget;
}

class ThisPartWidget;

class RunsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit RunsWidget(QWidget *parent = 0);
	~RunsWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(ThisPartWidget *part_widget);
private slots:
	void on_btDraw_clicked();
private:
	Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();

	Event::EventPlugin* eventPlugin();
private:
	enum class DrawMethod : int {Invalid = 0, RandomNumber};

	Ui::RunsWidget *ui;
	qf::core::model::SqlTableModel *m_runsModel;
	qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
};

#endif // RUNSWIDGET_H
