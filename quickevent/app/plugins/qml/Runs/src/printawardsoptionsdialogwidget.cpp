#include "printawardsoptionsdialogwidget.h"
#include "ui_printawardsoptionsdialogwidget.h"

#include <qf/core/log.h>

#include <QDirIterator>

PrintAwardsOptionsDialogWidget::PrintAwardsOptionsDialogWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::PrintAwardsOptionsDialogWidget)
{
	setPersistentSettingsId(objectName());
	ui->setupUi(this);
	ui->edNumPlaces->setValue(3);
}

PrintAwardsOptionsDialogWidget::~PrintAwardsOptionsDialogWidget()
{
	delete ui;
}

void PrintAwardsOptionsDialogWidget::init(const QString &plugin_home)
{
	qfLogFuncFrame() << "plugin home:" << plugin_home;
	QDirIterator it(plugin_home + "/reports/awards", QDirIterator::Subdirectories);
	while (it.hasNext()) {
		it.next();
		QFileInfo fi = it.fileInfo();
		if(fi.isFile()) {
			QString fn = fi.fileName();
			if(fn.endsWith(QLatin1String(".qml"), Qt::CaseInsensitive)) {
				fn = fn.mid(0, fn.length() - 4);
				ui->edReportPath->addItem(fn, fi.canonicalFilePath());
			}
		}
	}
}

QVariantMap PrintAwardsOptionsDialogWidget::printOptions() const
{
	QVariantMap ret;
	if(ui->edReportPath->currentIndex() >= 0) {
		ret["numPlaces"] = ui->edNumPlaces->value();
		ret["reportPath"] = ui->edReportPath->currentData().toString();
	}
	return ret;
}
