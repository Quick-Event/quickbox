#ifndef QF_QMLWIDGETS_FRAMEWORK_LOGWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_LOGWIDGET_H

#include "dockablewidget.h"

#include <qf/core/logcore.h>

#include <QTableView>

class QAbstractButton;

namespace qf {
namespace core {
class LogEntryMap;
namespace model { class LogTableModel; }
}
}

namespace qf {
namespace qmlwidgets {
namespace framework {

class LogFilterProxyModel;

namespace Ui {
class LogWidget;
}

class LogWidgetTableView : public QTableView
{
	Q_OBJECT
private:
	typedef QTableView Super;
public:
	LogWidgetTableView(QWidget *parent);

	void copy();
protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
};

class QFQMLWIDGETS_DECL_EXPORT LogWidget : public qf::qmlwidgets::framework::DockableWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DockableWidget Super;
public:
	explicit LogWidget(QWidget *parent = 0);
	~LogWidget();

	Q_SLOT void addLog(qf::core::Log::Level severity, const QString& category, const QString &file, int line, const QString& msg, const QDateTime& time_stamp, const QString &function, const QVariant &user_data = QVariant());
	Q_SLOT void addLogEntry(const qf::core::LogEntryMap &le);
	Q_SLOT void scrollToLastEntry();

	void clear();
	virtual void setLogTableModel(qf::core::model::LogTableModel *m);
	qf::core::model::LogTableModel* logTableModel();

	Q_SIGNAL void severityTresholdChanged(qf::core::Log::Level lvl);
	void setSeverityTreshold(qf::core::Log::Level lvl);
protected:
	QAbstractButton* tableMenuButton();
	QTableView* tableView() const;

	void clearCategoryActions();
	virtual void addCategoryActions(const QString &caption, const QString &id, qf::core::Log::Level level = qf::core::Log::Level::Invalid);
	QMap<QString, core::Log::Level> selectedLogCategories() const;
	virtual void registerLogCategories();

	virtual void onDockWidgetVisibleChanged(bool visible);
	void onVerticalScrollBarValueChanged();
private:
	Q_SLOT void onSeverityTresholdChanged(int index);
	Q_SLOT void filterStringChanged(const QString &filter_string);
	Q_SLOT void on_btClearLog_clicked();
	Q_SLOT void on_btResizeColumns_clicked();
protected:
	qf::core::model::LogTableModel* m_logTableModel = nullptr;
	LogFilterProxyModel* m_filterModel = nullptr;
private:
	Ui::LogWidget *ui;
	bool m_logCategoriesRegistered = false;

	QList<QAction*> m_logLevelActions;
	QList<QMenu*> m_loggingCategoriesMenus;
	bool m_columnsResized = false;
	bool m_isAutoScroll = true;
};


} // namespace framework
} // namespace qmlwiggets
} // namespace qf
#endif // QF_QMLWIDGETS_FRAMEWORK_LOGWIDGET_H
