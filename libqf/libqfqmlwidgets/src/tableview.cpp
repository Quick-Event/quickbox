#include "tableview.h"
#include "headerview.h"
#include "action.h"

#include <qf/core/string.h>
#include <qf/core/collator.h>
#include <qf/core/log.h>

#include <QKeyEvent>
#include <QMenu>
#include <QAbstractButton>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets;

TableView::TableView(QWidget *parent) :
	Super(parent)
{
	{
		HeaderView *h = new HeaderView(Qt::Horizontal, this);
		setHorizontalHeader(h);
		connect(this, &TableView::searchStringChanged, h, &HeaderView::setSearchString);
	}
	{
		HeaderView *h = new HeaderView(Qt::Vertical, this);
		setVerticalHeader(h);
	}
	setSortingEnabled(true);

	createActions();
	{
		/// top left corner actions
		foreach(QAbstractButton *bt, findChildren<QAbstractButton*>()) {
			if(bt->metaObject()->className() == QString("QTableCornerButton")) { /// src/gui/itemviews/qtableview.cpp:103
				//qfInfo() << "addidng actions";
				bt->setText("M");
				bt->setToolTip(trUtf8("Right click for menu."));
				bt->setContextMenuPolicy(Qt::ActionsContextMenu);
				QList<QAction*> lst;
				for(auto a : contextMenuActionsForGroups(AllActions))
					lst << a;
				bt->addActions(lst);
			};
		}
	}
}

qf::core::model::TableModel *TableView::tableModel() const
{
	return qobject_cast<qf::core::model::TableModel*>(Super::model());
}

void TableView::setTableModel(core::model::TableModel *m)
{
	qf::core::model::TableModel *old_m = tableModel();
	if (old_m != m) {
		Super::setModel(m);
		emit modelChanged();
	}
}

qf::core::utils::Table::SortDef TableView::seekSortDefinition() const
{
	qfLogFuncFrame();
	qf::core::utils::Table::SortDef ret;
	if(tableModel()) {
		ret = tableModel()->table().tableProperties().sortDefinition().value(0);
	}
	return ret;
}

int TableView::seekColumn() const
{
	int ret = -1;
	QHeaderView *h = horizontalHeader();
	if(h) {
		if(h->isSortIndicatorShown() && h->sortIndicatorOrder() == Qt::AscendingOrder) {
			ret = h->sortIndicatorSection();
		}
	}
	return ret;
}

void TableView::seek(const QString &prefix_str)
{
	qfLogFuncFrame() << prefix_str;
	if(prefix_str.isEmpty())
		return;
	if(!model())
		return;
	int col = seekColumn();
	if(col >= 0) {
		qf::core::Collator sort_collator = tableModel()->table().sortCollator();
		sort_collator.setCaseSensitivity(Qt::CaseInsensitive);
		sort_collator.setIgnorePunctuation(true);
		//qfWarning() << sort_collator.compare(QString::fromUtf8("s"), QString::fromUtf8("š")) << QString::fromUtf8("š").toUpper();
		//qfWarning() << "collator CS:" << (sort_collator.caseSensitivity() == Qt::CaseSensitive);
		for(int i=0; i<model()->rowCount(); i++) {
			QModelIndex ix = model()->index(i, col, QModelIndex());
			QString data_str = model()->data(ix, Qt::DisplayRole).toString();//.mid(0, prefix_str.length()).toLower();
			/// QTBUG-37689 QCollator allways sorts case sensitive
			/// workarounded by own implementation of qf::core::Collator
			QStringRef ps(&prefix_str);
			QStringRef ds(&data_str, 0, prefix_str.length());
			//QString ps = prefix_str.toLower();
			//QString ds = data_str.mid(0, ps.length()).toLower();
			int cmp = sort_collator.compare(ps, ds);
			//qfInfo() << ps << "cmp" << ds << "->" << cmp;
			if(cmp <= 0) {
				setCurrentIndex(ix);
				break;
			}
		}
	}
}

void TableView::keyPressEvent(QKeyEvent *e)
{
	qfLogFuncFrame() << "key:" << e->key() << "modifiers:" << e->modifiers();
	if(!model()) {
		e->ignore();
		return;
	}

	bool incremental_search = false;
	bool incremental_search_key_accepted = false;
	qfc::String old_seek_string = m_seekString;
	//bool modified = (e->modifiers() != Qt::NoModifier && e->modifiers() != Qt::KeypadModifier);
	bool key_enter = (e->key() == Qt::Key_Return && e->modifiers() == 0)
			|| (e->key() == Qt::Key_Enter && e->modifiers() == Qt::KeypadModifier);
	if(e->modifiers() == Qt::ControlModifier) {
		if(e->key() == Qt::Key_C) {
			//copy();
			//e->accept();
			//return;
		}
		else if(e->key() == Qt::Key_V) {
			//paste();
			//e->accept();
			//return;
		}
		else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
			qfDebug() << "\tCTRL+ENTER";
			////postRow();
			e->accept();
			return;
		}
	}
	else if(key_enter) {
		qfDebug() << "\t ENTER pressed";
	}
	else {
		qfu::Table::SortDef sd = seekSortDefinition();
		if(sd.isValid() && sd.ascending && seekColumn() >= 0 && (currentIndex().column() == seekColumn() || !currentIndex().isValid())) {
			incremental_search = true;
			if(!currentIndex().isValid())
				setCurrentIndex(model()->index(0, seekColumn(), QModelIndex()));
			//qfInfo() << "incremental search currentIndex row:" << currentIndex().row() << "col:" << currentIndex().column();
			/// Pokud je nektery sloupec serazen vzestupne zkusi se provest incremental search,
			/// pak se event dal nepropaguje
			QChar seekChar = qfc::String(e->text()).value(0);
			//bool is_valid_seek_char = true;
			if(e->key() == Qt::Key_Home
					|| e->key() == Qt::Key_End
					|| e->key() == Qt::Key_Left
					|| e->key() == Qt::Key_Up
					|| e->key() == Qt::Key_Right
					|| e->key() == Qt::Key_Down
					|| e->key() == Qt::Key_PageUp
					|| e->key() == Qt::Key_PageDown) {
				incremental_search = false;
				seekChar = QChar();
			}
			else if(seekChar == '\n' || seekChar == '\r')
				seekChar = QChar();
			qfDebug().nospace() << "\tincremental search seekChar unicode: 0x" << QString::number(seekChar.unicode(),16) << " key: 0x" << QString::number(e->key(),16);
			bool shift_only = (e->key() == Qt::Key_Shift);
			//bool accept = false;
			if(incremental_search) {
				if(e->key() == Qt::Key_Backspace) {
					m_seekString = old_seek_string.slice(0, -1);
					incremental_search_key_accepted = true;
				}
				else if(e->key() == Qt::Key_Escape) {
					m_seekString = QString();
					incremental_search_key_accepted = true;
				}
				else if(seekChar.isNull() && !shift_only) {
					m_seekString = QString();
				}
				else {
					m_seekString += seekChar;
					qfDebug() << "new seek text:" << m_seekString;
					incremental_search_key_accepted = true;
				}
				if(!m_seekString.isEmpty()) {
					if(m_seekString != old_seek_string) {
						seek(m_seekString);
					}
					incremental_search_key_accepted = true;
				}
			}
		}
	}
	if(!(incremental_search && incremental_search_key_accepted)) {
		m_seekString = QString();
	}
	if(m_seekString != old_seek_string)
		emit searchStringChanged(m_seekString);
	if(incremental_search && incremental_search_key_accepted) {
		qfDebug() << "\tUSED for incremental search";
		e->accept();
		return;
	}
	bool event_should_be_accepted = false;
	/// nejedna se o inkrementalni vyhledavani, zkusime editaci
	if(state() == EditingState) {
		qfDebug() << "\teditor exists";
		//QModelIndex current = currentIndex();
		//QModelIndex newCurrent;
		/// cursor keys moves selection, check editor data before
		/// some of switched keys shoul be filtered by editor
		qfDebug() << "\tHAVE EDITOR key:" << e->key();
		switch (e->key()) {
			case Qt::Key_Down:
			case Qt::Key_Up:
			case Qt::Key_Left:
			case Qt::Key_Right:
			case Qt::Key_Home:
			case Qt::Key_End:
			case Qt::Key_PageUp:
			case Qt::Key_PageDown:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
			case Qt::Key_Return:
			case Qt::Key_Enter:
				qfDebug() << "accepting event";
				/// je to trochu jedno, protoze to vypada, ze accept flag, kterej prijde dialogu je ten, jak ho nastavi editor (widget dodany delegatem) ve sve funkci keyPressEvent(...)
				//e->accept();
				event_should_be_accepted = true;
				/*****
				QFItemDelegate *it = qobject_cast<QFItemDelegate*>(itemDelegate());
				if(it) {
					if(!it->canCloseEditor()) return;
				}
				*/
				//qfDebug().color(QFLog::Yellow) << "set focus to table view";
				setFocus(); /// jinak se mi zavre delegat a focus skoci na jinej widget
				break;
		}
	}
	else {
		if(key_enter) {
			///timhle jsem zajistil, ze se editor otevira na enter a ne jen na F2, jak je defaultne v QT
			/// viz: QAbstractItemView::keyPressEvent(...)
			qfDebug() << "\tkey chci otevrit editor, state:" << state();
			if(edit(currentIndex(), EditKeyPressed, e)) {
				qfDebug() << "accepting event";
				e->accept();
			}
			else {
				qfDebug() << "ignoring event";
				e->ignore();
			}
			//qfDebug() << "accepting event";
			//e->accept();
			qfDebug() << "\t exiting after open editor try, event accepted:" << e->isAccepted() << "event:" << e;
			return;
		}
	}
	qfDebug() << "\tcalling parent implementation QTableView::keyPressEvent(e), state:" << state() << "event accepted:" << e->isAccepted();
	QTableView::keyPressEvent(e);
	/// parent implementace muze zmenit accepted() flag eventu
	qfDebug() << "\tcalled parent implementation QTableView::keyPressEvent(e), state:" << state() << "event accepted:" << e->isAccepted();
	if(event_should_be_accepted) e->accept();
}

void TableView::createActions()
{
	Action *a;
	{
		a = new Action(tr("Resize columns to contents"), this);
		//a->setIcon(QIcon(":/libqf/images/reload.png"));
		//a->setShortcut(QKeySequence(tr("Ctrl+R", "reload SQL table")));
		//a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("resizeColumnsToContents");
		m_actionGroups[SizeActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, &Action::triggered, this, &TableView::resizeColumnsToContents);
	}
	{
		a = new Action(tr("Reload"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/reload.png"));
		a->setShortcut(QKeySequence(tr("Ctrl+R", "reload SQL table")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("reload");
		m_actionGroups[ViewActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(reload()));
	}
	{
		a = new Action(tr("Copy"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/copy.png"));
		a->setShortcut(QKeySequence(tr("Ctrl+C", "Copy selection")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("copy");
		m_actionGroups[ViewActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(copy()));
	}
	{
		a = new Action(tr("Copy special"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/copy.png"));
		//a->setShortcut(QKeySequence(tr("Ctrl+C", "Copy selection")));
		//a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("copySpecial");
		m_actionGroups[ViewActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(copySpecial()));
	}
	{
		a = new Action(tr("Paste"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/paste.png"));
		a->setShortcut(QKeySequence(tr("Ctrl+V", "Paste rows")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("paste");
		m_actionGroups[PasteActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(paste()), Qt::QueuedConnection); /// hazelo mi to vyjjimky v evendloopu
	}
	{
		a = new Action(QIcon(":/qf/qmlwidgets/images/new.png"), tr("Insert row"), this);
		a->setShortcut(QKeySequence(tr("Ctrl+Ins", "insert row SQL table")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("insertRow");
		m_actionGroups[RowActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(insertRow()));
	}
	{
		a = new Action(QIcon(":/qf/qmlwidgets/images/delete.png"), tr("Delete selected rows"), this);
		a->setShortcut(QKeySequence(tr("Ctrl+Del", "delete row SQL table")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("removeSelectedRows");
		m_actionGroups[RowActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));
	}
	{
		a = new Action(tr("Post row edits"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/sql_post.png"));
		a->setShortcut(QKeySequence(tr("Ctrl+Return", "post row SQL table")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("postRow");
		m_actionGroups[RowActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(postRow()));
	}
	{
		a = new Action(tr("Revert row edits"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/revert.png"));
		a->setShortcut(QKeySequence(tr("Ctrl+Z", "revert edited row")));
		a->setShortcutContext(Qt::WidgetShortcut);
		a->setOid("revertRow");
		m_actionGroups[RowActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(revertRow()));
	}
	{
		a = new Action(tr("Copy row"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/clone.png"));
		a->setOid("copyRow");
		a->setVisible(false);
		m_actionGroups[RowActions] << a->oid();
		a->setShortcut(QKeySequence(tr("Ctrl+D", "insert row copy")));
		a->setShortcutContext(Qt::WidgetShortcut);
		m_actionGroups[RowActions] << a->oid();
		m_actions[a->oid()] = a;
		connect(a, SIGNAL(triggered()), this, SLOT(copyRow()));
	}
	{
		a = new Action(tr("Zobrazit ve formulari"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/view.png"));
		a->setToolTip(tr("Zobrazit radek v formulari pro cteni"));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(emitViewRowInExternalEditor()));
		a->setOid("viewRowExternal");
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Upravit ve formulari"), this);
		a->setIcon(QIcon(":/qf/qmlwidgets/images/edit.png"));
		a->setToolTip(tr("Upravit radek ve formulari"));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(emitEditRowInExternalEditor()));
		a->setOid("editRowExternal");
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(QIcon(":/qf/qmlwidgets/images/sort-asc.png"), tr("Sort ascending"), this);
		a->setOid("sortAsc");
		a->setCheckable(true);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		a->setShortcutContext(Qt::WidgetShortcut);
		m_actionGroups[SortActions] << a->oid();
		connect(a, SIGNAL(triggered(bool)), this, SLOT(sortAsc(bool)));
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(QIcon(":/qf/qmlwidgets/images/sort-desc.png"), tr("Sort descending"), this);
		a->setOid("sortDesc");
		a->setCheckable(true);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		//a->setShortcutContext(Qt::WidgetShortcut);
		m_actionGroups[SortActions] << a->oid();
		connect(a, SIGNAL(triggered(bool)), this, SLOT(sortDesc(bool)));
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Edit cell content"), this);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		a->setShortcut(QKeySequence(tr("Ctrl+Shift+T", "Edit cell content")));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(showCurrentCellText()));
		a->setOid("showCurrentCellText");
		m_actionGroups[CellActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Save BLOB"), this);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		//a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(saveCurrentCellBlob()));
		a->setOid("saveCurrentCellBlob");
		m_actionGroups[BlobActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Load BLOB from file"), this);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		//a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(loadCurrentCellBlob()));
		a->setOid("loadCurrentCellBlob");
		m_actionGroups[BlobActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Insert rows statement"), this);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		//a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(insertRowsStatement()));
		a->setOid("insertRowsStatement");
		m_actionGroups[RowActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Set NULL in selection"), this);
		//a->setToolTip(tr("Upravit radek v externim editoru"));
		a->setShortcut(QKeySequence(tr("Ctrl+Shift+L", "Set NULL in selection")));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(setNullInSelection()));
		a->setOid("setNullInSelection");
		m_actionGroups[SetValueActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Set value in selection"), this);
		a->setShortcut(QKeySequence(tr("Ctrl+Shift+E", "Set value in selection")));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(setValueInSelection()));
		a->setOid("setValueInSelection");
		m_actionGroups[SetValueActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Generate sequence in selection"), this);
		//a->setShortcut(QKeySequence(tr("Ctrl+Shift+E", "Set value in selection")));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(generateSequenceInSelection()));
		a->setOid("generateSequenceInSelection");
		m_actionGroups[SetValueActions] << a->oid();
		m_actions[a->oid()] = a;
	}
	{
		a = new Action(tr("Select"), this);
		a->setOid("select");
		m_actionGroups[SelectActions] << a->oid();
		m_actions[a->oid()] = a;
		QMenu *m = new QMenu(this);
		a->setMenu(m);
		{
			a = new Action(tr("Select current column"), this);
			a->setShortcutContext(Qt::WidgetShortcut);
			connect(a, SIGNAL(triggered()), this, SLOT(selectCurrentColumn()));
			a->setOid("selectCurrentColumn");
			m->addAction(a);
		}
		{
			a = new Action(tr("Select current row"), this);
			a->setShortcutContext(Qt::WidgetShortcut);
			connect(a, SIGNAL(triggered()), this, SLOT(selectCurrentRow()));
			a->setOid("selectCurrentRow");
		}
		m->addAction(a);
	}
	{
		a = new Action(tr("Calculate"), this);
		a->setOid("calculate");
		m_actionGroups[CalculateActions] << a->oid();
		m_actions[a->oid()] = a;
		QMenu *m = new QMenu(this);
		a->setMenu(m);
		{
			a = new Action(tr("Sum column"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(sumColumn()));
			a->setOid("sumColumn");
			m->addAction(a);
		}
		{
			a = new Action(tr("Sum selection"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(sumSelection()));
			a->setOid("sumSelection");
			m->addAction(a);
		}
	}
	{
		a = new Action(tr("Export"), this);
		a->setOid("export");
		m_actionGroups[ExportActions] << a->oid();
		m_actions[a->oid()] = a;
		QMenu *m = new QMenu(this);
		a->setMenu(m);
		{
			a = new Action(tr("Report"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(exportReport()));
			a->setOid("exportReport");
			m_actions[a->oid()] = a;
			m->addAction(a);
		}
		{
			a = new Action(tr("CSV"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(exportCSV()));
			a->setOid("exportCSV");
			m_actions[a->oid()] = a;
			m->addAction(a);
		}
		{
			a = new Action(tr("HTML"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(exportHTML()));
			a->setOid("exportHTML");
			m_actions[a->oid()] = a;
			m->addAction(a);
		}
//#ifdef QF_XLSLIB
		{
			a = new Action(tr("XLS - MS Excel"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(exportXLS()));
			a->setOid("exportXLS");
			m_actions[a->oid()] = a;
			m->addAction(a);
		}
//#endif
		{
			a = new Action(tr("XML (MS Excel 2003)"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(exportXML()));
			a->setOid("exportXML");
			m_actions[a->oid()] = a;
			m->addAction(a);
		}
	}
	{
		a = new Action(tr("Import"), this);
		a->setOid("import");
		m_actionGroups[ImportActions] << a->oid();
		m_actions[a->oid()] = a;
		QMenu *m = new QMenu(this);
		a->setMenu(m);
		{
			a = new Action(tr("CSV"), this);
			connect(a, SIGNAL(triggered()), this, SLOT(importCSV()));
			a->setOid("importCSV");
			m_actions[a->oid()] = a;
			m->addAction(a);
		}
	}
	/*
	f_toolBarActions << action("insertRow");
	f_toolBarActions << action("copyRow");
	f_toolBarActions << action("removeSelectedRows");
	f_toolBarActions << action("postRow");
	f_toolBarActions << action("revertRow");
	//f_toolBarActions << action("insertRowExternal");
	f_toolBarActions << action("viewRowExternal");
	f_toolBarActions << action("editRowExternal");
	a = new Action(this); a->setSeparator(true);
	f_toolBarActions << a;
	f_toolBarActions << action("reload");
	a = new Action(this); a->setSeparator(true);
	f_toolBarActions << a;
	f_toolBarActions << action("sortAsc");
	f_toolBarActions << action("sortDesc");
	a = new Action(this); a->setSeparator(true);
	f_toolBarActions << a;
	f_toolBarActions << action("filter");

	f_contextMenuActions = standardContextMenuActions();
	*/
	{
		for(Action *a : m_actions) {
			if(!a->shortcut().isEmpty()) {
				//qfInfo() << "\t inserting action" << a->text() << a->shortcut().toString();
				addAction(a); /// aby chodily shortcuty, musi byt akce pridany widgetu
			}
		}
		//qfTrash() << "\t default actions inserted";
	}
}

QList<Action *> TableView::contextMenuActionsForGroups(int action_groups)
{
	qfLogFuncFrame();
	//static Action *act_separator = nullptr;
	QList<Action*> ret;
	QList<int> grps;
	grps << SizeActions << SortActions << FilterActions << ViewActions << PasteActions << RowActions << BlobActions << SetValueActions << CellActions << SelectActions << CalculateActions << ExportActions << ImportActions;
	int cnt = 0;
	foreach(int grp, grps) {
		if(action_groups & grp) {
			QStringList sl = m_actionGroups.value(grp);
			if(!sl.isEmpty() && (cnt++ > 0)) {
				Action *act_separator = m_separatorsForGroup.value((ActionGroup)grp);
				if(!act_separator) {
					act_separator = new Action(this);
					act_separator->setSeparator(true);
					m_separatorsForGroup[(ActionGroup)grp] = act_separator;
				}
				act_separator = m_separatorsForGroup.value((ActionGroup)grp);
				ret << act_separator;
			}
			for(auto oid : sl) {
				Action *a = m_actions.value(oid);
				if(a == nullptr)
					qfWarning() << QString("Cannot find action for oid: '%1'").arg(oid);
				else
					ret << a;
			}
		}
	}
	return ret;
}
