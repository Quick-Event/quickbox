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

QString SqlTableItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
	if(value.userType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto dbe = value.value<qf::core::sql::DbEnum>();
		return dbe.caption();
	}
	return Super::displayText(value, locale);
}

QWidget *SqlTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QVariant v = index.data();
	if(v.userType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto *editor = new QComboBox(parent);
		auto m = qobject_cast<qf::core::model::SqlTableModel*>(view()->tableModel());
		if(m) {
			auto cache = qf::core::sql::DbEnumCache::instance(m->connectionName());
			auto dbe = v.value<qf::core::sql::DbEnum>();
			QString group_name = dbe.groupName();
			Q_FOREACH(auto e, cache.dbEnumsForGroup(group_name)) {
				editor->addItem(e.caption(), e.groupId());
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
	QVariant v = index.data();
	if(v.userType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
		auto dbe = v.value<qf::core::sql::DbEnum>();
		auto cbx = qobject_cast<QComboBox*>(editor);
		if(cbx) {
			int ix = cbx->findData(dbe.groupId());
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
	QVariant v = index.data();
	if(v.userType() == qMetaTypeId<qf::core::sql::DbEnum>()) {
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
