#include "tableitemdelegate.h"

#include "tableview.h"

#include <qf/core/assert.h>

#include <QPainter>

using namespace qf::qmlwidgets;

TableItemDelegate::TableItemDelegate(TableView *parent) :
	Super(parent)
{
}

TableView * TableItemDelegate::view() const
{
	TableView *view = qobject_cast<TableView*>(parent());
	QF_ASSERT_EX(view != nullptr, "Cannot use TableItemDelegate without TableView parent");
	return view;
}

void TableItemDelegate::paintBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	/**
	Protoze z nepochopitelnyho duvodu neni funkce drawBackground() virtualni, musim patchovat QItemDelegate::drawBackground() v QT, kdyz chci podsvitit aktivni radek
	qtitemdelegate.cpp:823

	#if 1/// QF_PATCH patch pro podsviceni radku se selekci
		QVariant value = index.data(Qt::BackgroundRole);
		if(!value.isValid()) value = property("qfSelectedRowHighlightColor");
	#else
		QVariant value = index.data(Qt::BackgroundRole);
	#endif

	*/
	TableView *v = view();
	if(v) {
		QModelIndex ix = v->currentIndex();
		if(index.row() == ix.row() && index != ix) {
			/// fill current row background
			/// da se to udelat i takhle bez patchovani QT
			/// pozor, aby to fungovalo musi se jeste v TableView::currentChanged() volat updateRow() na radky u kterych se meni selekce
			static const QColor sel_row_background1(245, 245, 184);
			static const QColor sel_row_background2(210, 240, 184);
			painter->fillRect(option.rect, (v->inlineEditSaveStrategy() == TableView::OnEditedValueCommit)? sel_row_background2: sel_row_background1);
		}
		else {
			/// fill background of RO cells
			Qt::ItemFlags flags = index.flags();
			//qfInfo() << "col:" << index.column() << "editable:" << f.contains(Qt::ItemIsEditable);
			if(!(flags & Qt::ItemIsEditable)) {
				static const QColor ro_cell_background("#eeeeff");
				painter->fillRect(option.rect, ro_cell_background);
			}
		}
	}
}

void TableItemDelegate::paintForeground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Super::paint(painter, option, index);
}

void TableItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	paintBackground(painter, option, index);
	paintForeground(painter, option, index);
}

QWidget *TableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QWidget *ret = Super::createEditor(parent, option, index);
	return ret;
}
