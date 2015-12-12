#include "logwidget.h"
#include "ui_logwidget.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>
#include <qf/core/model/logtablemodel.h>

#include <QSortFilterProxyModel>
#include <QDateTime>

namespace qf {
namespace qmlwidgets {
namespace framework {

class LogFilterProxyModel : public QSortFilterProxyModel
{
	typedef QSortFilterProxyModel Super;
public:
	LogFilterProxyModel(QObject* parent)
		: Super(parent) {

	}
	void setThreshold(int level) {
		m_treshold = level;
		invalidateFilter();
	}
	void setFilterString(const QString &filter_string) {
		m_filterString = filter_string;
		invalidateFilter();
	}
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
private:
	int m_treshold = static_cast<int>(qf::core::Log::Level::Info);
	QString m_filterString;
};

bool LogFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	auto *log_model = qobject_cast<qf::core::model::LogTableModel*>(sourceModel());
	QModelIndex index = log_model->index(source_row, qf::core::model::LogTableModel::TableRow::Severity, source_parent);
	bool ok = (log_model->data(index, Qt::EditRole).toInt() <= m_treshold);
	//qfDebug() << log_model->data(index, Qt::EditRole).toInt() << m_treshold << ok;
	if(ok && !m_filterString.isEmpty()) {
		ok = false;
		index = log_model->index(source_row, 0, source_parent);
		for (int i = 0; i < qf::core::model::LogTableModel::TableRow::Cols::Count; ++i) {
			index = index.sibling(index.row(), i);
			QString s = log_model->data(index, Qt::DisplayRole).toString();
			if(s.contains(m_filterString, Qt::CaseInsensitive)) {
				ok = true;
				break;
			}
		}
	}
	return ok;
}

LogWidget::LogWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::LogWidget)
{
	ui->setupUi(this);
	//setPersistentSettingsId();

	//ui->tableView->horizontalHeader()->setSectionHidden(0, true);
	ui->tableView->horizontalHeader()->setSectionsMovable(true);
	ui->tableView->verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	//ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableView->setModel(m_filterModel = new LogFilterProxyModel(this));

	for (int i = static_cast<int>(qf::core::Log::Level::Error); i <= static_cast<int>(qf::core::Log::Level::Debug); i++) {
		ui->severityTreshold->addItem(qf::core::Log::levelToString(static_cast<qf::core::Log::Level>(i)), QVariant::fromValue(i));
	}
	connect(ui->severityTreshold, SIGNAL(currentIndexChanged(int)), this, SLOT(tresholdChanged(int)));
	connect(ui->edFilter, &QLineEdit::textChanged, this, &LogWidget::filterStringChanged);
	ui->severityTreshold->setCurrentIndex(static_cast<int>(qf::core::Log::Level::Info));
}

LogWidget::~LogWidget()
{
	delete ui;
}

void LogWidget::onDockWidgetVisibleChanged(bool visible)
{
	Super::onDockWidgetVisibleChanged(visible);
	if(visible) {
		m_filterModel->setSourceModel(logTableModel());
		ui->tableView->setModel(m_filterModel);
		ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	}
	else {
		// disconnect view when hidden, this can spped up app start significantly
		ui->tableView->setModel(nullptr);
	}
}

void LogWidget::clear()
{
	QF_SAFE_DELETE(m_logTableModel);
}

void LogWidget::addLogEntry(const qf::core::LogEntryMap &le)
{
	logTableModel()->addLogEntry(le.level(), le.category(), le.file(), le.line(), le.message(), QDateTime::currentDateTime());
	ui->tableView->scrollToBottom();
}

qf::core::model::LogTableModel *LogWidget::logTableModel()
{
	if(!m_logTableModel) {
		m_logTableModel = new qf::core::model::LogTableModel(this);
	}
	return m_logTableModel;
}

bool LogWidget::isModelLoaded() const
{
	return m_logTableModel != nullptr;
}

void LogWidget::tresholdChanged(int index)
{
	Q_UNUSED(index);
	m_filterModel->setThreshold(ui->severityTreshold->currentData().toInt());
}

void LogWidget::filterStringChanged(const QString &filter_string)
{
	m_filterModel->setFilterString(filter_string);
}

void LogWidget::on_btClearEvents_clicked()
{
	logTableModel()->clear();
}

void LogWidget::on_btResizeColumns_clicked()
{
	ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

QAbstractButton *LogWidget::tableMenuButton()
{
	return ui->btTableMenu;
}

} // namespace framework
} // namespace qmlwiggets
} // namespace qf
