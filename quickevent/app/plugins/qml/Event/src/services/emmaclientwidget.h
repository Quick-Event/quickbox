#ifndef EMMACLIENTWIDGET_H
#define EMMACLIENTWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

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
	explicit EmmaClientWidget(QWidget *parent = 0);
	~EmmaClientWidget();
private:
	void onBtChooseExportDirClicked();
	void onBtExportSplitsClicked();
	bool acceptDialogDone(int result) override;
	EmmaClient* service();
	void saveSettings();
private:
	Ui::EmmaClientWidget *ui;
};

}

#endif // EMMACLIENTWIDGET_H
