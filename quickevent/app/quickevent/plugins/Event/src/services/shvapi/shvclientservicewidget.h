#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Event::services::shvapi {

namespace Ui {
class ShvClientServiceWidget;
}

class ShvClientService;

class ShvClientServiceWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit ShvClientServiceWidget(QWidget *parent = nullptr);
	~ShvClientServiceWidget();
private:
	ShvClientService* service();
	bool saveSettings();
	void updateStarterToolUrl();
private:
	Ui::ShvClientServiceWidget *ui;
	bool acceptDialogDone(int result);
};

}

