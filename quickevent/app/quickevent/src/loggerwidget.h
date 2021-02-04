#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include <qf/qmlwidgets/framework/logwidget.h>

namespace qf { namespace core { class LogEntryMap; }}
namespace qf { namespace core { namespace model { class LogEntryMap; }}}

class LoggerLogDevice;

class LoggerWidget : public qf::qmlwidgets::framework::LogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::LogWidget Super;
public:
	explicit LoggerWidget(QWidget *parent = 0);
	~LoggerWidget() Q_DECL_OVERRIDE;

	void onDockWidgetVisibleChanged(bool visible) Q_DECL_OVERRIDE;
private:
	void registerLogCategories() Q_DECL_OVERRIDE;
private:
	qf::core::model::LogTableModel *m_logModel = nullptr;
};

#endif // LOGGERWIDGET_H
