#include "printtableviewwidget.h"
#include "ui_printtableviewwidget.h"
#include "../../../tableview.h"

namespace qfc = qf::core;

using namespace qf::qmlwidgets::reports;

PrintTableViewWidget::PrintTableViewWidget(TableView *table_view, QWidget *parent) :
	Super(parent),
	IPersistentOptions(this),
	ui(new Ui::PrintTableViewWidget)
{
	ui->setupUi(this);
	qfc::model::TableModel *model = table_view->tableModel();
	ui->columnsChooser->setVisible(model);
	ui->columnsChooser->loadColumns(model);

	ui->saveSettingsWidget->setOptionsProvider(this);
	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

PrintTableViewWidget::~PrintTableViewWidget()
{
	delete ui;
}

void PrintTableViewWidget::loadPersistentOptions()
{
	ui->saveSettingsWidget->load(true);
}

void PrintTableViewWidget::applyOptions()
{
	QVariant opts = persistentOptions();
	emit printRequest(opts);
}

QVariant PrintTableViewWidget::persistentOptions()
{
	QVariantMap ret;
	ret["options"] = exportOptions();
	ret["columns"] = ui->columnsChooser->exportedColumns();
	return ret;
}

void PrintTableViewWidget::setPersistentOptions(const QVariant &opts)
{
	QVariantMap m = opts.toMap();
	setExportOptions(m.value("options").toMap());
	ui->columnsChooser->setExportedColumns(m.value("columns"));
}

QVariant PrintTableViewWidget::exportOptions() const
{
	QVariantMap ret;
	{
		QVariantMap m1;
		m1["title"] = reportTitle();
		m1["note"] = reportNote();
		m1["fileName"] = reportFileName();
		ret["report"] = m1;
	}
	{
		QVariantMap m1;
		m1["selectedRowsOnly"] = isSelectedRowsOnly();
		ret["options"] = m1;
	}
	return ret;
}

void PrintTableViewWidget::setExportOptions(const QVariant& opts) const
{
	QVariantMap m = opts.toMap();
	ui->edReportTitle->setText(m.value("report").toMap().value("title").toString());
	ui->edNote->setText(m.value("report").toMap().value("note").toString());
	ui->edReportFileName->setText(m.value("report").toMap().value("fileName").toString());
	ui->chkSelectedRowsOnly->setChecked(m.value("options").toMap().value("selectedRowsOnly").toBool());
}

QString PrintTableViewWidget::reportFileName() const
{
	QString ret = ui->edReportFileName->text();
	return ret;
}

bool PrintTableViewWidget::isSelectedRowsOnly() const
{
	return ui->chkSelectedRowsOnly->isChecked();
}

QString PrintTableViewWidget::reportTitle() const
{
	return ui->edReportTitle->text();
}

QString PrintTableViewWidget::reportNote() const
{
	return ui->edNote->toPlainText();
}
