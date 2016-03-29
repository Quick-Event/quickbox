#include "logwidget.h"
#include "ui_logwidget.h"

#include "../style.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/logdevice.h>
#include <qf/core/model/logtablemodel.h>

#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QClipboard>
#include <QKeyEvent>
#include <QMenu>
#include <QInputDialog>
#include <QScrollBar>

namespace qfm = qf::core::model;

namespace qf {
namespace qmlwidgets {
namespace framework {

LogWidgetTableView::LogWidgetTableView(QWidget *parent)
	: Super(parent)
{
	auto *style = qf::qmlwidgets::Style::instance();
	QAction *a;
	{
		a = new QAction(tr("Copy"), this);
		a->setIcon(style->icon("copy"));
		a->setShortcut(QKeySequence(tr("Ctrl+C", "Copy selection")));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, &QAction::triggered, this, &LogWidgetTableView::copy);
		addAction(a);
	}
	setContextMenuPolicy(Qt::ActionsContextMenu);
}

void LogWidgetTableView::copy()
{
	qfLogFuncFrame();
	auto *m = model();
	if(!m)
		return;
	int n = 0;
	QString rows;
	QItemSelection sel = selectionModel()->selection();
	foreach(const QItemSelectionRange &sel1, sel) {
		if(sel1.isValid()) {
			for(int row=sel1.top(); row<=sel1.bottom(); row++) {
				QString cells;
				for(int col=sel1.left(); col<=sel1.right(); col++) {
					QModelIndex ix = m->index(row, col);
					QString s;
					s = ix.data(Qt::DisplayRole).toString();
					static constexpr bool replace_escapes = true;
					if(replace_escapes) {
						s.replace('\r', QStringLiteral("\\r"));
						s.replace('\n', QStringLiteral("\\n"));
						s.replace('\t', QStringLiteral("\\t"));
					}
					if(col > sel1.left())
						cells += '\t';
					cells += s;
				}
				if(n++ > 0)
					rows += '\n';
				rows += cells;
			}
		}
	}
	if(!rows.isEmpty()) {
		qfDebug() << "\tSetting clipboard:" << rows;
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(rows);
	}
}

void LogWidgetTableView::keyPressEvent(QKeyEvent *e)
{
	qfLogFuncFrame() << "key:" << e->key() << "modifiers:" << e->modifiers();
	if(e->modifiers() == Qt::ControlModifier) {
		if(e->key() == Qt::Key_C) {
			copy();
			e->accept();
			return;
		}
	}
	Super::keyPressEvent(e);
}

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
	auto *log_model = qobject_cast<qfm::LogTableModel*>(sourceModel());
	QModelIndex index = log_model->index(source_row, qfm::LogTableModel::Cols::Severity, source_parent);
	bool ok = (log_model->data(index, Qt::EditRole).toInt() <= m_treshold);
	//qfDebug() << log_model->data(index, Qt::EditRole).toInt() << m_treshold << ok;
	if(ok && !m_filterString.isEmpty()) {
		ok = false;
		index = log_model->index(source_row, 0, source_parent);
		for (int i = 0; i < qfm::LogTableModel::Cols::Count; ++i) {
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
	{
		QAction *a = new QAction(tr("Maximal log length"), this);
		connect(a, &QAction::triggered, [this]() {
			qf::core::model::LogTableModel *m = this->logTableModel();
			int max_rows = m->maximumRowCount();
			bool ok;
			max_rows = QInputDialog::getInt(this, tr("Get number"), tr("Maximal log row count:"), max_rows, 0, std::numeric_limits<int>::max(), 100, &ok);
			if (ok)
				m->setMaximumRowCount(max_rows);
		});
		tableMenuButton()->addAction(a);
	}
	{
		QAction *a = new QAction(this);
		a->setSeparator(true);
		tableMenuButton()->addAction(a);
	}

	//ui->tableView->horizontalHeader()->setSectionHidden(0, true);
	ui->tableView->horizontalHeader()->setSectionsMovable(true);
	ui->tableView->verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	m_filterModel = new LogFilterProxyModel(this);
	m_filterModel->setDynamicSortFilter(false);
	ui->tableView->setModel(m_filterModel);

	for (int i = static_cast<int>(qf::core::Log::Level::Error); i <= static_cast<int>(qf::core::Log::Level::Debug); i++) {
		ui->severityTreshold->addItem(qf::core::Log::levelToString(static_cast<qf::core::Log::Level>(i)), QVariant::fromValue(i));
	}
	connect(ui->severityTreshold, SIGNAL(currentIndexChanged(int)), this, SLOT(tresholdChanged(int)));
	ui->severityTreshold->setCurrentIndex(static_cast<int>(qf::core::Log::Level::Info));

	connect(ui->edFilter, &QLineEdit::textChanged, this, &LogWidget::filterStringChanged);
}

LogWidget::~LogWidget()
{
	delete ui;
}

void LogWidget::clear()
{
	if(m_logTableModel)
		m_logTableModel->clear();
}

void LogWidget::setLogTableModel(core::model::LogTableModel *m)
{
	if(m_logTableModel != m) {
		m_logTableModel = m;
		m_filterModel->setSourceModel(m_logTableModel);
		if(m_logTableModel) {
			connect(m_logTableModel, &core::model::LogTableModel::logEntryInserted, this, &LogWidget::scrollToLastEntry, Qt::UniqueConnection);
			QScrollBar *sb = ui->tableView->verticalScrollBar();
			if(sb)
				connect(sb, &QScrollBar::sliderReleased, this, &LogWidget::onSliderReleased, Qt::UniqueConnection);
		}
	}
}

void LogWidget::addLog(core::Log::Level severity, const QString &category, const QString &file, int line, const QString &msg, const QDateTime &time_stamp, const QString &function, const QVariant &user_data)
{
	//fprintf(stderr, "%s:%d(%s) %s\n", qPrintable(file), line, qPrintable(category), qPrintable(msg));
	logTableModel()->addLog(severity, category, file, line, msg, time_stamp, function, user_data);
}

void LogWidget::addLogEntry(const qf::core::LogEntryMap &le)
{
	addLog(le.level(), le.category(), le.file(), le.line(), le.message(), le.timeStamp(), le.function());
}

qf::core::model::LogTableModel *LogWidget::logTableModel()
{
	if(!m_logTableModel) {
		auto *m = new qfm::LogTableModel(this);
		setLogTableModel(m);
	}
	return m_logTableModel;
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

void LogWidget::on_btClearLog_clicked()
{
	clear();
}

void LogWidget::on_btResizeColumns_clicked()
{
	ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

QAbstractButton *LogWidget::tableMenuButton()
{
	return ui->btTableMenu;
}

QTableView *LogWidget::tableView() const
{
	return ui->tableView;
}

void LogWidget::clearCategoryActions()
{
	m_logLevelActions.clear();
	qDeleteAll(m_loggingCategoriesMenus);
	m_loggingCategoriesMenus.clear();
}

void LogWidget::addCategoryActions(const QString &caption, const QString &id, core::Log::Level level)
{
	QString menu_caption = "[%1] " + caption;
	QMenu *m = new QMenu(this);
	m_loggingCategoriesMenus << m;
	QAction *a = new QAction(caption, m);
	a->setData(id);
	a->setMenu(m);
	tableMenuButton()->addAction(a);
	QActionGroup *ag_loglevel = new QActionGroup(a);
	for (int i = static_cast<int>(qf::core::Log::Level::Invalid); i <= static_cast<int>(qf::core::Log::Level::Debug); i++) {
		if(i == static_cast<int>(qf::core::Log::Level::Fatal))
			continue;
		QString cap = qf::core::Log::levelToString(static_cast<qf::core::Log::Level>(i));
		QAction *a2 = new QAction(cap, a);
		ag_loglevel->addAction(a2);
		m->addAction(a2);
		a2->setCheckable(true);
		a2->setChecked(static_cast<qf::core::Log::Level>(i) == level);
		connect(a2, &QAction::triggered, this, &LogWidget::registerLogCategories);
		QChar level_c = (i == static_cast<int>(qf::core::Log::Level::Invalid))? ' ': qf::core::Log::levelName(static_cast<qf::core::Log::Level>(i))[0];
		a2->setData(i);
		if(a2->isChecked())
			menu_caption = menu_caption.arg(level_c);
		m_logLevelActions << a2;
	}
	m->setTitle(menu_caption);
}

QMap<QString, qf::core::Log::Level> LogWidget::selectedLogCategories() const
{
	QMap<QString, qf::core::Log::Level> categories;
	for(QAction *a : m_logLevelActions) {
		if(a->isChecked()) {
			QAction *pa = qobject_cast<QAction*>(a->parent());
			QF_ASSERT(pa != nullptr, "Bad parent", continue);
			qf::core::Log::Level level = static_cast<qf::core::Log::Level>(a->data().toInt());
			QChar level_c = (level == qf::core::Log::Level::Invalid)? ' ': qf::core::Log::levelName(level)[0];
			{
				QMenu *m = pa->menu();
				QString cap = m->title();
				cap[1] = level_c;
				m->setTitle(cap);
			}
			QString category = pa->data().toString();
			QPair<QString, qf::core::Log::Level> lev = qf::core::LogDevice::parseCategoryLevel(category);
			if(level != qf::core::Log::Level::Invalid)
				categories[category] = level;
		}
	}
	return categories;
}

void LogWidget::registerLogCategories()
{
	m_logCategoriesRegistered = true;
}

void LogWidget::onDockWidgetVisibleChanged(bool visible)
{
	if(visible) {
		if(!m_logCategoriesRegistered) {
			registerLogCategories();
		}
		scrollToLastEntry();
	}
}

void LogWidget::onSliderReleased()
{
	QScrollBar *sb = ui->tableView->verticalScrollBar();
	if(sb) {
		if(logTableModel()->direction() == qf::core::model::LogTableModel::Direction::AppendToBottom) {
			m_isAutoScroll = (sb->value() == sb->maximum());
			//fprintf(stderr, "BOTTOM scrollbar min: %d max: %d value: %d -> %d\n", sb->minimum(), sb->maximum(), sb->value(), m_scrollToLastEntryAfterInsert);
		}
		else {
			m_isAutoScroll = (sb->value() == sb->minimum());
			//fprintf(stderr, "TOP scrollbar min: %d max: %d value: %d -> %d\n", sb->minimum(), sb->maximum(), sb->value(), m_scrollToLastEntryAfterInsert);
		}
	}
}

void LogWidget::scrollToLastEntry()
{
	if(isVisible()) {
		if(!m_columnsResized) {
			m_columnsResized = true;
			ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
		}
		if(m_isAutoScroll) {
			if(logTableModel()->direction() == qf::core::model::LogTableModel::Direction::AppendToBottom) {
				ui->tableView->scrollToBottom();
			}
			else {
				ui->tableView->scrollToTop();
			}
		}
	}
}

} // namespace framework
} // namespace qmlwiggets
} // namespace qf
