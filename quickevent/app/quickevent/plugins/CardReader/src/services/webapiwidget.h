#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace CardReader {
namespace services {

namespace Ui {
	class WebApiWidget;
}

class WebApi;

class WebApiWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit WebApiWidget(QWidget *parent = nullptr);
	~WebApiWidget() override;
private:
	bool acceptDialogDone(int result) override;
	WebApi* service();
	void saveSettings();
	void onBtChooseLogFileClicked();
private:
	Ui::WebApiWidget *ui;
};

}}
