#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Event {
namespace services {

namespace Ui {
class EmmaClientWidget;
}

class EmmaClient;

class EmmaClientWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit EmmaClientWidget(QWidget *parent = nullptr);
	~EmmaClientWidget();
private:
	void onBtChooseExportDirClicked();
	void onBtExportSplitsClicked();
	void onBtExportFinishClicked();
	void onBtExportStartClicked();
	void onBtExportXML30Clicked();
	void onBtExportStartXML30Clicked();
	bool acceptDialogDone(int result) override;
	EmmaClient* service();
	bool saveSettings();
private:
	Ui::EmmaClientWidget *ui;
};

}}

