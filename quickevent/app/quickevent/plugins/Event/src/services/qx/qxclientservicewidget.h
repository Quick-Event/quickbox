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
	~QxClientServiceWidget() override;
private:
	void setMessage(const QString &msg = {}, bool is_error = false);
	QxClientService* service();
	bool saveSettings();
	void updateOCheckListPostUrl();
private:
	Ui::QxClientServiceWidget *ui;
	bool acceptDialogDone(int result) override;
};

}

