#ifndef EVENTSTATISTICSWIDGET_H
#define EVENTSTATISTICSWIDGET_H

#include <QWidget>

namespace qf { namespace core { namespace model { class SqlTableModel; }}}

namespace Ui {
class EventStatisticsWidget;
}

class FooterView;
class FooterModel;
class EventStatisticsModel;

class EventStatisticsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EventStatisticsWidget(QWidget *parent = nullptr);
	~EventStatisticsWidget();

	Q_SLOT void reloadDeferred();
	void reload();
	Q_SLOT void onDbEventNotify(const QString &domain, const QVariant &payload);
	Q_SLOT void onVisibleChanged(bool is_visible);

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
private slots:
	void on_btReload_clicked();
	void on_btOptions_clicked();
	void on_btPrintResultsSelected_clicked();
	void on_btPrintResultsNew_clicked();
	void on_btClearNewInSelectedRows_clicked();
private:
	int currentStageId();
	void initAutoRefreshTimer();
	//QTimer* printResultsTimer();
	void clearNewResults(const QList<int> &classdefs_ids, const QList<int> &runners_finished);

	QVariantMap options();
	void printResultsForRows(const QList<int> &rows);
private:
	Ui::EventStatisticsWidget *ui;
	FooterView *m_tableFooterView = nullptr;
	FooterModel *m_tableFooterModel = nullptr;
	EventStatisticsModel *m_tableModel = nullptr;
	QTimer *m_reloadLaterTimer = nullptr;
	QTimer *m_autoRefreshTimer = nullptr;
	//QTimer *m_printResultsTimer = nullptr;
};

#endif // EVENTSTATISTICSWIDGET_H
