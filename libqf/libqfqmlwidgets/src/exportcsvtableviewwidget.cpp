#include "exportcsvtableviewwidget.h"
#include "ui_exporttableviewwidget.h"

#include <qf/core/model/tablemodel.h>

namespace qf {
namespace qmlwidgets {

ExportCsvTableViewWidget::ExportCsvTableViewWidget(QTableView *table_view, QWidget *parent)
	: Super(table_view, parent)
{

}

QVariant ExportCsvTableViewWidget::exportOptions() const
{
	qf::core::utils::Table::TextExportOptions ret(Super::exportOptions().toMap());
	if(ui->btSeparatorTab->isChecked()) {
		ret.setFieldSeparator("\t");
	}
	else {
		QString fs = ui->edSeparatorOther->text().trimmed();
		ret.setFieldSeparator(fs);
	}
	{
		QString fs = ui->edQuoteChar->text().trimmed();
		//qfInfo() << fs.length() << fs.toUtf8().toHex();
		ret.setFieldQuotes(fs);
	}
	if(ui->btQuotesAlways->isChecked())
		ret.setFieldQuotingPolicy(qf::core::utils::Table::TextExportOptions::Always);
	else if(ui->btQuotesNever->isChecked())
		ret.setFieldQuotingPolicy(qf::core::utils::Table::TextExportOptions::Never);
	else
		ret.setFieldQuotingPolicy(qf::core::utils::Table::TextExportOptions::IfNecessary);
	ret.setCodecName(ui->lstCodec->currentText());
	return ret;
}

void ExportCsvTableViewWidget::setExportOptions(const QVariant& opts) const
{
	Super::setExportOptions(opts);
	qf::core::utils::Table::TextExportOptions text_opts(opts.toMap());
	if(text_opts.fieldSeparator() == "\t")
		ui->btSeparatorTab->setChecked(true);
	else
		ui->rbSeparatorOther->setChecked(true);
	if(!ui->btSeparatorTab->isChecked())
		ui->edSeparatorOther->setText(text_opts.fieldSeparator().trimmed());
	ui->edQuoteChar->setText(text_opts.fieldQuotes().trimmed());
	ui->btQuotesAlways->setChecked(text_opts.fieldQuotingPolicy() == qf::core::utils::Table::TextExportOptions::Always);
	ui->btQuotesNever->setChecked(text_opts.fieldQuotingPolicy() == qf::core::utils::Table::TextExportOptions::Never);
	ui->btQuotesIfNecessary->setChecked(text_opts.fieldQuotingPolicy() == qf::core::utils::Table::TextExportOptions::IfNecessary);
	ui->lstCodec->setCurrentIndex(ui->lstCodec->findText(text_opts.codecName()));
}

} // namespace qmlwidgets
} // namespace qf

