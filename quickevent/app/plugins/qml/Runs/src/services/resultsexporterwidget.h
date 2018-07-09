#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace services {

namespace Ui {
class ResultsExporterWidget;
}

class ResultsExporter;

class ResultsExporterWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT

	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit ResultsExporterWidget(QWidget *parent = 0);
	~ResultsExporterWidget();
private:
	void onBtChooseExportDir();
	bool acceptDialogDone(int result) override;
	ResultsExporter* service();
private:
	Ui::ResultsExporterWidget *ui;
};

}

