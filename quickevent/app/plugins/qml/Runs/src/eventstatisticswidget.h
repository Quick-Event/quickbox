#ifndef EVENTSTATISTICSWIDGET_H
#define EVENTSTATISTICSWIDGET_H

#include <QWidget>

namespace qf { namespace core { namespace model { class SqlTableModel; }}}

namespace Ui {
class EventStatisticsWidget;
}

class EventStatisticsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit EventStatisticsWidget(QWidget *parent = 0);
	~EventStatisticsWidget();

	void reload();
	Q_SLOT void onDbEvent(const QString &domain, const QVariant &payload);
private:
	int currentStageId();
private:
	Ui::EventStatisticsWidget *ui;
	qf::core::model::SqlTableModel *m_tableModel = nullptr;
};

#endif // EVENTSTATISTICSWIDGET_H
