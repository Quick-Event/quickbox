#include "tableview.h"
#include "headerview.h"

#include <qf/core/string.h>
#include <qf/core/log.h>

#include <QKeyEvent>

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
		QCollator sort_collator = tableModel()->table().sortCollator();
		/// QTBUG-37689 QCollator allways sorts case sensitive
		sort_collator.setCaseSensitivity(Qt::CaseInsensitive);
		//qfInfo() << "collator CS:" << (sortCollator.caseSensitivity() == Qt::CaseSensitive);
		for(int i=0; i<model()->rowCount(); i++) {
			QModelIndex ix = model()->index(i, col, QModelIndex());
			QString data_str = model()->data(ix, Qt::DisplayRole).toString();//.mid(0, prefix_str.length()).toLower();
			QStringRef ps(&prefix_str);
			QStringRef ds(&data_str, 0, prefix_str.length());
			int cmp = sort_collator.compare(ps, ds);
			//qfInfo() << ba1 << "cmp" << ba2 << "from:" << model()->data(ix, Qt::DisplayRole).toString() << "->" << cmp << ba2.at(ba2.length()-1).isPunct();
			if(cmp == 0) {
				/// nemuzu tu mit cmp == 0
				/// protoze collator.cmpare("s", "š") == -1, ikdyz ignorePunctuation == true, asi chyba
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
