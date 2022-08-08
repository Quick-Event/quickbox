#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Event {
namespace services {

namespace Ui {
class OResultsClientWidget;
}

class OResultsClient;

class OResultsClientWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit OResultsClientWidget(QWidget *parent = nullptr);
	~OResultsClientWidget();
private:
	void onBtExportResultsXml30Clicked();
	void onBtExportStartListXml30Clicked();
	OResultsClient* service();
	bool saveSettings();
private:
	Ui::OResultsClientWidget *ui;
};

}}

