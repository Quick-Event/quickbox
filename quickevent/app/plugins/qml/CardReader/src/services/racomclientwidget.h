#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace services {

namespace Ui {
class RacomClientWidget;
}

class RacomClient;

class RacomClientWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit RacomClientWidget(QWidget *parent = 0);
	~RacomClientWidget();
private:
	bool acceptDialogDone(int result) override;
	RacomClient* service();
	void saveSettings();
private:
	Ui::RacomClientWidget *ui;
};

}

