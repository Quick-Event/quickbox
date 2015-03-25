#include "sqltableitemdelegate.h"
#include "tableview.h"

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/dbenumcache.h>

#include <QComboBox>

using namespace qf::qmlwidgets;

SqlTableItemDelegate::SqlTableItemDelegate(TableView *parent)
	: Super(parent)
{

}

QWidget *SqlTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const auto dbenum_scheme = qf::core::model::TableModel::ColumnDefinition::DBENUM_SCHEME;
	QString format = index.data(qf::core::model::TableModel::DisplayFormatRole).toString();
	if(format.startsWith(dbenum_scheme)) {
		auto *editor = new QComboBox(parent);
		auto m = qobject_cast<qf::core::model::SqlTableModel*>(view()->tableModel());
		if(m) {
			auto cache = qf::core::sql::DbEnumCache::instance(m->connectionName());
			QString group_name = format.mid(dbenum_scheme.length());
			for(auto dbe : cache.dbEnumsForGroup(group_name)) {
				editor->addItem(dbe.caption(), dbe.groupId());
			}
		}
		connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
		return editor;
	}
	else {
		return Super::createEditor(parent, option, index);
	}
}

void SqlTableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString format = index.data(qf::core::model::TableModel::DisplayFormatRole).toString();
	if(format.startsWith(qf::core::model::TableModel::ColumnDefinition::DBENUM_SCHEME)) {
		QString group_id = index.data(qf::core::model::TableModel::RawValueRole).toString();
		auto cbx = qobject_cast<QComboBox*>(editor);
		if(cbx) {
			int ix = cbx->findData(group_id);
			cbx->setCurrentIndex(ix);
			cbx->showPopup();
		}
	}
	else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void SqlTableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QString format = index.data(qf::core::model::TableModel::DisplayFormatRole).toString();
	if(format.startsWith(qf::core::model::TableModel::ColumnDefinition::DBENUM_SCHEME)) {
		auto cbx = qobject_cast<QComboBox*>(editor);
		if(cbx) {
			QString group_id = cbx->currentData().toString();
			model->setData(index, group_id);
		}
	} else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void SqlTableItemDelegate::commitAndCloseEditor()
{
	auto editor = qobject_cast<QComboBox*>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
	view()->setFocus();
}
