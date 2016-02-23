#ifndef EVENTSTATISTICSWIDGET_H
#define EVENTSTATISTICSWIDGET_H

#include <QWidget>

namespace qf { namespace core { namespace model { class SqlTableModel; }}}

namespace Ui {
class EventStatisticsWidget;
}

class QHeaderView;
class EventStatisticsModel;

class EventStatisticsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit EventStatisticsWidget(QWidget *parent = 0);
	~EventStatisticsWidget();

	void reload();
	Q_SLOT void onDbEvent(const QString &domain, const QVariant &payload);
private slots:
	void on_btReload_clicked();
	void onSectionResized(int logical_index, int old_size, int new_size);
	void syncFooterSectionSizes();
private:
	int currentStageId();
private:
	Ui::EventStatisticsWidget *ui;
	QHeaderView *m_tableFooter = nullptr;
	EventStatisticsModel *m_tableModel = nullptr;
};

#endif // EVENTSTATISTICSWIDGET_H
