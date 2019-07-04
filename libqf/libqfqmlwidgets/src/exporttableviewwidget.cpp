#include "exporttableviewwidget.h"
#include "ui_exporttableviewwidget.h"
#include "headerview.h"

#include <qf/core/utils/table.h>

#include <QMenu>
#include <QTextCodec>

namespace qf {
namespace qmlwidgets {

ExportTableViewWidget::ExportTableViewWidget(QTableView *table_view, QWidget *parent)
	: Super(parent)
	, IPersistentOptions(this)
	, ui(new Ui::ExportTableViewWidget)
{
	ui->setupUi(this);

	foreach(QByteArray ba, QTextCodec::availableCodecs())
		ui->lstCodec->addItem(ba);
	ui->lstCodec->setCurrentText(QStringLiteral("UTF-8"));

	ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableView->verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	ui->tableView->setModel(table_view->model());

	ui->saveSettingsWidget->setOptionsProvider(this);

	QHeaderView *hhdr = new HeaderView(Qt::Horizontal, this);
	ui->tableView->setHorizontalHeader(hhdr);

	hhdr->setSectionsClickable(false);
}

ExportTableViewWidget::~ExportTableViewWidget()
{
	delete ui;
}

void ExportTableViewWidget::loadPersistentOptions()
{
	ui->saveSettingsWidget->load(true);
}

void ExportTableViewWidget::applyOptions()
{
	QVariant opts = exportOptions();
	emit exportRequest(opts);
}

QVariant ExportTableViewWidget::persistentOptions()
{
	QVariantMap ret;
	ret["options"] = exportOptions();
	QHeaderView *hdr = ui->tableView->horizontalHeader();
	QByteArray header_state = hdr->saveState();
	ret["tableHeaderState"] = QString::fromLatin1(header_state.toBase64());
	return ret;
}

void ExportTableViewWidget::setPersistentOptions(const QVariant &opts)
{
	QVariantMap m = opts.toMap();
	setExportOptions(m.value("options").toMap());
	QByteArray header_state = m.value("tableHeaderState").toString().toLatin1();
	header_state = QByteArray::fromBase64(header_state);
	ui->tableView->horizontalHeader()->restoreState(header_state);
}

QVariant ExportTableViewWidget::exportOptions() const
{
	qf::core::utils::Table::TextExportOptions ret;
	ret.setExportColumnNames(ui->chkHeader->isChecked());
	ret.setFullColumnNames(true);
	//ret.setFullColumnNames(ui->chkFullColumnNames->isChecked());
	//ret.setUseColumnCaptions(ui->chkUseColumnsCaptions->isChecked());
	ret.setFromLine(ui->edFromLine->value());
	ret.setToLine(ui->edToLine->value());
	QVariantList cols;
	{
		QHeaderView *hdr = ui->tableView->horizontalHeader();
		for (int i = 0; i < hdr->count(); ++i) {
			int log_ix = hdr->logicalIndex(i);
			if(!hdr->isSectionHidden(log_ix))
				cols << log_ix;
		}
	}
	ret["columns"] = cols;
	return ret;
}

void ExportTableViewWidget::setExportOptions(const QVariant &opts) const
{
	qf::core::utils::Table::TextExportOptions text_opts(opts.toMap());
	ui->chkHeader->setChecked(text_opts.isExportColumnNames());
	//ui->chkFullColumnNames->setChecked(opts.isFullColumnNames());
	//ui->chkUseColumnsCaptions->setChecked(opts.isUseColumnCaptions());
	ui->edFromLine->setValue(text_opts.fromLine());
	ui->edToLine->setValue(text_opts.toLine());
}

} // namespace qmlwidgets
} // namespace qf
