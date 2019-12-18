#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace CardReader {
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
	explicit RacomClientWidget(QWidget *parent = nullptr);
	~RacomClientWidget() override;
private:
	bool acceptDialogDone(int result) override;
	RacomClient* service();
	void saveSettings();
private:
	Ui::RacomClientWidget *ui;
};

}}

