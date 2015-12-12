#ifndef QF_QMLWIDGETS_FRAMEWORK_LOGWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_LOGWIDGET_H

#include "dockablewidget.h"

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

class QFQMLWIDGETS_DECL_EXPORT LogWidget : public qf::qmlwidgets::framework::DockableWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DockableWidget Super;
public:
	explicit LogWidget(QWidget *parent = 0);
	~LogWidget();
	void onDockWidgetVisibleChanged(bool visible) Q_DECL_OVERRIDE;
	void clear();

	Q_SLOT void addLogEntry(const qf::core::LogEntryMap &le);
protected:
	qf::core::model::LogTableModel* logTableModel();
	bool isModelLoaded() const;
	Q_SLOT void tresholdChanged(int index);
	Q_SLOT void filterStringChanged(const QString &filter_string);
	QAbstractButton* tableMenuButton();
private slots:
	void on_btClearEvents_clicked();
	void on_btResizeColumns_clicked();
private:
	Ui::LogWidget *ui;
	qf::core::model::LogTableModel* m_logTableModel = nullptr;
	LogFilterProxyModel* m_filterModel = nullptr;
};


} // namespace framework
} // namespace qmlwiggets
} // namespace qf
#endif // QF_QMLWIDGETS_FRAMEWORK_LOGWIDGET_H
