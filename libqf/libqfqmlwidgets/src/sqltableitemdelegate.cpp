#include "sqltableitemdelegate.h"
#include "tableview.h"

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/dbenumcache.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QComboBox>

using namespace qf::qmlwidgets;

SqlTableItemDelegate::SqlTableItemDelegate(TableView *parent)
	: Super(parent)
{

}
/*
QString SqlTableItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
	if(value.userType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto dbe = value.value<qf::core::sql::DbEnum>();
		return dbe.caption();
	}
	return Super::displayText(value, locale);
}
*/
QWidget *SqlTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QVariant v = index.data(qf::core::model::TableModel::ColumnDefinitionRole);
	qf::core::model::TableModel::ColumnDefinition cd = v.value<qf::core::model::TableModel::ColumnDefinition>();
	if(cd.castType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto *editor = new QComboBox(parent);
		auto m = qobject_cast<const qf::core::model::SqlTableModel*>(view()->tableModel());
		if(m) {
			qf::core::model::SqlTableModel::DbEnumCastProperties props(cd.castProperties());
			qf::core::sql::DbEnumCache& db_enum_cache = qf::core::sql::DbEnumCache::instanceForConnection(m->connectionName());
			Q_FOREACH(auto dbe, db_enum_cache.dbEnumsForGroup(props.groupName())) {
				QString cap = dbe.fillInPlaceholders(props.captionFormat());
				editor->addItem(cap, dbe.groupId());
				QColor c = dbe.color();
				if(c.isValid()) {
					editor->setItemData(editor->count() - 1, c, Qt::BackgroundRole);
					editor->setItemData(editor->count() - 1, qf::core::model::TableModel::contrastTextColor(c), Qt::TextColorRole);
				}
			}
		}
		//connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
		return editor;
	}
	else {
		return Super::createEditor(parent, option, index);
	}
}

void SqlTableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QVariant v = index.data(qf::core::model::TableModel::ColumnDefinitionRole);
	qf::core::model::TableModel::ColumnDefinition cd = v.value<qf::core::model::TableModel::ColumnDefinition>();
	if(cd.castType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto cbx = qobject_cast<QComboBox*>(editor);
		if(cbx) {
			QString group_id = index.data(Qt::EditRole).toString();
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
	QVariant v = index.data(qf::core::model::TableModel::ColumnDefinitionRole);
	qf::core::model::TableModel::ColumnDefinition cd = v.value<qf::core::model::TableModel::ColumnDefinition>();
	if(cd.castType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto cbx = qobject_cast<QComboBox*>(editor);
		if(cbx) {
			QString group_id = cbx->currentData().toString();
			model->setData(index, group_id);
		}
	}
	else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}
/*
void SqlTableItemDelegate::commitAndCloseEditor()
{
	auto *editor = qobject_cast<QWidget*>(sender());
	QF_CHECK(editor != nullptr, "Editor is not a QWidget!");
	emit commitData(editor);
	emit closeEditor(editor);
	view()->setFocus();
}
*/
