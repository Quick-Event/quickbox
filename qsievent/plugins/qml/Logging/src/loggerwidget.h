#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include <QFrame>

namespace Ui {
class LoggerWidget;
}

namespace qf {
namespace core {
class SignalLogDevice;
}
}

class LoggerWidget : public QFrame
{
	Q_OBJECT
public:
	explicit LoggerWidget(QWidget *parent = 0);
	~LoggerWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void onLogEntry(const QVariantMap &log_entry);
	Q_SLOT void onLogLevelSet(int ix);
private:
	Ui::LoggerWidget *ui;
	qf::core::SignalLogDevice *m_logDevice;
};

#endif // LOGGERWIDGET_H
