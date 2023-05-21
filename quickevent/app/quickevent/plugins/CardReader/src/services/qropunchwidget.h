#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace CardReader {
namespace services {

namespace Ui {
	class QrOPunchWidget;
}

class QrOPunch;

class QrOPunchWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit QrOPunchWidget(QWidget *parent = nullptr);
	~QrOPunchWidget() override;
private:
	bool acceptDialogDone(int result) override;
	QrOPunch* service();
	void saveSettings();
	void onBtChooseLogFileClicked();
private:
	Ui::QrOPunchWidget *ui;
};

}}
