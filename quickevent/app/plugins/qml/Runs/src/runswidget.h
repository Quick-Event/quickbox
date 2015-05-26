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
class RunsTableModel;
class RunsTableItemDelegate;

class RunsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit RunsWidget(QWidget *parent = 0);
	~RunsWidget() Q_DECL_OVERRIDE;

	static Event::EventPlugin* eventPlugin();

	void settleDownInPartWidget(ThisPartWidget *part_widget);
private slots:
	void on_btDraw_clicked();
	void on_btDrawRemove_clicked();

private:
	int currentStageId();

	Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();

	/**
	 * @brief runnersInClubsHistogram
	 * @return list of runs.id for each club sorted by their count, longest list of runners is first
	 */
	QList< QList<int> > runnersByClubSortedByCount(int stage_id, int class_id, QMap<int, QString> &runner_id_to_club);
	QList<int> runnersForClass(int stage_id, int class_id);
private:
	enum class DrawMethod : int {Invalid = 0, RandomNumber, EquidistantClubs, RandomizedEquidistantClubs};

	Ui::RunsWidget *ui;
	RunsTableModel *m_runsModel;
	RunsTableItemDelegate *m_runsTableItemDelegate;
	qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
};

#endif // RUNSWIDGET_H
