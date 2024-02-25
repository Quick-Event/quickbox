#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Event::services::shvapi {

namespace Ui {
class ClientWidget;
}

class Client;

class ClientWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit ClientWidget(QWidget *parent = nullptr);
	~ClientWidget();
private:
	Client* service();
	bool saveSettings();
private:
	Ui::ClientWidget *ui;
	bool acceptDialogDone(int result);
};

}

