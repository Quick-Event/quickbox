#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Event::services::qx {

namespace Ui {
class QxClientServiceWidget;
}

class QxClientService;

class QxClientServiceWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit QxClientServiceWidget(QWidget *parent = nullptr);
	~QxClientServiceWidget();
private:
	QxClientService* service();
	bool saveSettings();
private:
	Ui::QxClientServiceWidget *ui;
	bool acceptDialogDone(int result);
};

}

