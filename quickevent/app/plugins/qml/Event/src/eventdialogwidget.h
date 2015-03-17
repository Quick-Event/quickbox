#ifndef EVENTDIALOGWIDGET_H
#define EVENTDIALOGWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class EventDialogWidget;
}

class EventDialogWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit EventDialogWidget(QWidget *parent = 0);
	~EventDialogWidget() Q_DECL_OVERRIDE;

	void setEventId(const QString &event_id);
	QString eventId() const;

	void loadParams(const QVariantMap &params);
	QVariantMap saveParams();
private:
	Ui::EventDialogWidget *ui;
};

#endif // EVENTDIALOGWIDGET_H
