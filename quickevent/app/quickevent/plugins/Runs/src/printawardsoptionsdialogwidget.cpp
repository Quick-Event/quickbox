#include "printawardsoptionsdialogwidget.h"
#include "ui_printawardsoptionsdialogwidget.h"
#include "runsplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>

#include <QDirIterator>

PrintAwardsOptionsDialogWidget::PrintAwardsOptionsDialogWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::PrintAwardsOptionsDialogWidget)
{
	setPersistentSettingsId(objectName());
	ui->setupUi(this);
	ui->edNumPlaces->setValue(3);

	auto *runs_plugin = qf::qmlwidgets::framework::getPlugin<Runs::RunsPlugin>();
	for(const auto &i : runs_plugin->listReportFiles("awards")) {
		qfDebug() << i.reportName << i.reportFilePath;
		ui->edReportPath->addItem(i.reportName, i.reportFilePath);
	}
}

PrintAwardsOptionsDialogWidget::~PrintAwardsOptionsDialogWidget()
{
	delete ui;
}

QVariantMap PrintAwardsOptionsDialogWidget::printOptions() const
{
	QVariantMap ret;
	if(ui->edReportPath->currentIndex() >= 0) {
		ret["numPlaces"] = ui->edNumPlaces->value();
		ret["stageId"] = ui->edStageNumber->value();
		ret["reportPath"] = ui->edReportPath->currentData().toString();
		qfDebug() << "reportPath:" << ui->edReportPath->currentData().toString();
	}
	return ret;
}

void PrintAwardsOptionsDialogWidget::setPrintOptions(const QVariantMap &opts)
{
	ui->edNumPlaces->setValue(opts.value("numPlaces").toInt());
	ui->edStageNumber->setValue(opts.value("stageId").toInt());
}
