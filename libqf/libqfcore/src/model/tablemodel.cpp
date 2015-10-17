#include "tablemodel.h"
#include "../core/log.h"
#include "../core/assert.h"
#include "../core/utils.h"
#include "../utils/treetable.h"

#include <QTime>
#include <QColor>
#include <QPixmap>
#include <QIcon>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;
using namespace qf::core::model;

//=========================================
//        TableModel::ColumnDefinition
//=========================================
const TableModel::ColumnDefinition & TableModel::ColumnDefinition::sharedNull()
{
	static ColumnDefinition n = ColumnDefinition(SharedDummyHelper());
	return n;
}

bool TableModel::ColumnDefinition::matchesSqlId(const QString column_name) const
{
	return qf::core::Utils::fieldNameEndsWith(fieldName(), column_name);
}

//=========================================
//        TableModel
//=========================================
QString TableModel::m_defaultTimeFormat = QStringLiteral("hh:mm:ss");
QString TableModel::m_defaultDateFormat = QStringLiteral("yyyy-MM-dd");
QString TableModel::m_defaultDateTimeFormat = QStringLiteral("yyyy-MM-ddThh:mm:ss");

TableModel::TableModel(QObject *parent) :
	Super(parent), m_autoColumns(false), m_nullReportedAsString(true)
{
}

void TableModel::clearRows()
{
	if(rowCount() > 0) {
		beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
		m_table.clearRows();
		endRemoveRows();
	}
}

int TableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_table.rowCount();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_columns.count();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	if(index.isValid()) {
		bool can_edit = false;
		ColumnDefinition cd = m_columns.value(index.column());
		if(!cd.isNull()) {
			can_edit = !cd.isReadOnly();
			if(!cd.isVirtual()) {
				qfu::Table::Field field = tableField(index.column());
				if(!field.isNull()) {
					can_edit = can_edit && field.canUpdate();
					int type = columnType(index.column());
					// BLOB fields cannot be edited in grid.
					//can_edit = can_edit && (type != QVariant::ByteArray);
					if(type == QVariant::Bool) {
						flags |= Qt::ItemIsUserCheckable;// << Qt::ItemIsEnabled;
					}
				}
				else {
					can_edit = false;
				}
			}
		}
		if(can_edit)
			flags |= Qt::ItemIsEditable;
		else
			flags &= ~Qt::ItemIsEditable;
		//qfInfo() << cd.fieldName() << flags << "checkable:" << (flags & Qt::ItemIsUserCheckable);
	}
	return flags;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret;
	if(!index.isValid())
		return ret;
	if(role == RawValueRole) {
		ret = value(index.row(), index.column());
	}
	else if(role == ValueIsDirtyRole) {
		ret = isDirty(index.row(), index.column());
	}
	else if(role == SortRole) {
		int type = columnType(index.column());
		if(type == QVariant::Bool)
			ret = value(index.row(), index.column()).toBool();
		else
			ret = data(index, Qt::DisplayRole);
	}
	else if(role == Qt::DisplayRole) {
		ColumnDefinition cd = m_columns.value(index.column());
		if(cd.isNull()) {
			return QString("!%1").arg(index.column());
		}
		if(data(index, ValueIsNullRole).toBool()) {
			if(isNullReportedAsString())
				return QStringLiteral("null");
			return QVariant();
		}
		ret = data(index, Qt::EditRole);
		int type = columnType(index.column());
		if(type == QVariant::Invalid)
			type = ret.type(); /// pokud jsou sloupce virtualni (sloupce se pocitaji, nemusi byt pro ne definovan typ)
		if(type == QVariant::ByteArray) {
			const static QString blob_string = "{blob %1%2}";
			int size = ret.toByteArray().size();
			if(size < 1024) ret = blob_string.arg(size).arg(" B");
			else ret = blob_string.arg(size/1024).arg("kB");
		}
		else if(type == QVariant::Bool) {
			/// display check
			ret = QString();
		}
		QString format = cd.format();
		if(format.isEmpty()) {
			if(type == QVariant::Date) {
				format = m_defaultDateFormat;
			}
			else if(type == QVariant::Time) {
				format = m_defaultTimeFormat;
				//qfInfo() << "format" << format;
			}
			else if(type == QVariant::DateTime) {
				format = m_defaultDateTimeFormat;
				//qfInfo() << "format" << format;
			}
		}
		if(!format.isEmpty()) {
			if(type == QVariant::Time) {
				QTime t = ret.toTime();
				ret = t.toString(format);
			}
			else if(type == QVariant::Date) {
				QDate d = ret.toDate();
				ret = d.toString(format);
			}
			else if(type == QVariant::DateTime) {
				QDateTime dt = ret.toDateTime();
				ret = dt.toString(format);
			}
		}
	}
	else if(role == Qt::EditRole) {
		ret = data(index, RawValueRole);
		ret = rawValueToEdit(index.column(), ret);
	}
	else if (role == ValueIsNullRole) {
		ret = data(index, RawValueRole);
		return ret.isNull() && ret.isValid();
	}
	else if (role == Qt::TextAlignmentRole) {
		const ColumnDefinition cd = m_columns.value(index.column());
		Qt::Alignment al = cd.alignment();
		if(!!al)
			ret = (int)al;
		else {
			if(!data(index, ValueIsNullRole).toBool()) {
				ret = data(index, RawValueRole);
				if(ret.type() > QVariant::Bool && ret.type() <= QVariant::Double)
					ret = Qt::AlignRight;
				else
					ret = Qt::AlignLeft;
			}
		}
	}
	else if(role == Qt::TextColorRole) {
		int type = columnType(index.column());
		if(type == QVariant::ByteArray)
			return QColor(Qt::blue);
		if(data(index, ValueIsNullRole).toBool()) {
			return QColor(Qt::blue);
		}
		ret = QVariant();
	}
	else if (role == Qt::BackgroundColorRole) {
		/// delegate does it
	}
	else if (role == Qt::CheckStateRole) {
		int type = columnType(index.column());
		if(type == QVariant::Bool) {
			//qfInfo() << "BOOL";
			return (data(index, Qt::EditRole).toBool()? Qt::Checked: Qt::Unchecked);
		}
		return QVariant();
	}
	else if (role == Qt::ToolTipRole) {
		QString s = data(index, Qt::DisplayRole).toString();
		//s = "<pre>" + s + "</pre>"; /// kvuli chybe v Qt 4.6.3, kdy aplikace pada pokud je v textu za sebou 0x09 0x0A
		s.replace("\t\n", "\n");
		ret = s;
		//ret = data(index, Qt::DisplayRole);
		//qfInfo() << ret.toString();
	}
	return ret;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	static QIcon ico_dirty(QPixmap(":/qf/core/images/pencil.png"));
	//static QIcon ico_filter(QPixmap(":/libqfgui/images/filter.png"));
	QVariant ret;
	if(orientation == Qt::Horizontal) {
		if(role == Qt::DisplayRole || role == Qt::EditRole) {
			ColumnDefinition cd = m_columns.value(section);
			QF_ASSERT(!cd.isNull(),
					  QString("Invalid horizontal section number: %1").arg(section),
					  return ret);
			ret = cd.caption();
		}
		else if(role == Qt::DecorationRole) {
		}
		else if(role == Qt::ToolTipRole) {
			ColumnDefinition cd = m_columns.value(section);
			ret = cd.toolTip();
		}
		else if (role == FieldTypeRole) {
			int type = columnType(section);
			return type;
		}
		else if(role == ColumnDefinitionRole) {
			ColumnDefinition cd = m_columns.value(section);
			ret = QVariant::fromValue(cd);
		}
		else if(role == FieldNameRole) {
			ret = m_columns.value(section).fieldName();
		}
	}
	else if(orientation == Qt::Vertical) {
		if(role == Qt::DisplayRole || role == Qt::EditRole) {
			qfu::TableRow r = m_table.row(section);
			if(!r.isNull() && r.isDirty()) {
				ret = QString();
			}
			else {
				ret = QVariant(section + 1);
			}
		}
		else if(role == Qt::DecorationRole) {
			qfu::TableRow r = m_table.row(section);
			if(!r.isNull() && r.isDirty()) {
				return qVariantFromValue(ico_dirty);
			}
		}
	}
	else {
		ret = QAbstractTableModel::headerData(section, orientation, role);
	}
	return ret;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	qfLogFuncFrame() << value << "role:" << role;
	bool ret = false;
	if(!index.isValid())
		return ret;
	if(role == Qt::EditRole) {
		QVariant val = editValueToRaw(index.column(), value);
		ret = setValue(index.row(), index.column(), val);
		if(ret)
			emit dataChanged(index, index);
	}
	else if(role == Qt::CheckStateRole) {
		//qfInfo() << __LINE__;
		bool chk_val = (value.toInt() == Qt::Unchecked)? 0: 1;
		ret = setValue(index.row(), index.column(), chk_val);
		if(ret)
			emit dataChanged(index, index);
	}
	else if(role == ValueIsDirtyRole) {
		bool d = value.toBool();
		ret = setDirty(index.row(), index.column(), d);
	}
	else {
		ret = Super::setData(index, value, role);
	}
	return ret;
}

void TableModel::sort(int column, Qt::SortOrder order)
{
	int table_field_index = tableFieldIndex(column);
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find table field index for column index: %1").arg(column),
			  return);
	qfu::Table::SortDef sd(table_field_index, order == Qt::AscendingOrder, !qfu::Table::SortDef::CaseSensitive);
	m_table.sort(sd);
	emit layoutChanged();
}

void TableModel::checkColumns()
{
	if(m_columns.isEmpty() || m_autoColumns) {
		createColumnsFromTableFields();
	}
	fillColumnIndexes();
}

bool TableModel::reload()
{
	beginResetModel();
	checkColumns();
	endResetModel();
	return true;
}

bool TableModel::postRow(int row_no, bool throw_exc)
{
	qfLogFuncFrame() << row_no << throw_exc;
	qfu::TableRow &row = m_table.rowRef(row_no);
	row.clearEditFlags();
	return true;
}

bool TableModel::postAll(bool throw_exc)
{
	bool ok = true;
	for (int row_no = 0; row_no < rowCount(); ++row_no) {
		qfu::TableRow row = m_table.row(row_no);
		if(row.isDirty()) {
			ok = postRow(row_no, throw_exc);
			if(!ok)
				break;
		}
	}
	return ok;
}

void TableModel::revertRow(int row_no)
{
	qfu::TableRow &row = m_table.rowRef(row_no);
	row.restoreOrigValues();
}

void TableModel::revertAll()
{
	for (int row_no = 0; row_no < rowCount(); ++row_no) {
		qfu::TableRow row = m_table.row(row_no);
		if(row.isDirty()) {
			revertRow(row_no);
		}
	}
}

int TableModel::reloadRow(int row_no)
{
	qfu::TableRow &row = m_table.rowRef(row_no);
	row.clearEditFlags();
	return 1;
}

bool TableModel::prepareForCopyRow(int row_no)
{
	qfu::TableRow &row = m_table.rowRef(row_no);
	row.prepareForCopy();
	return true;
}

void TableModel::setNullReportedAsString(bool arg)
{
	if (m_nullReportedAsString != arg) {
		m_nullReportedAsString = arg;
		emit nullReportedAsStringChanged(arg);
	}
}

bool TableModel::setValue(int row, int column, const QVariant &val)
{
	bool ret = false;
	QF_ASSERT(m_table.isValidRowIndex(row),
			  tr("Invalid table row: %1").arg(row),
			  return ret);
	int table_field_index = tableFieldIndex(column);
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find table field index for column index: %1").arg(column),
			  return ret);
	QF_ASSERT(m_table.isValidFieldIndex(table_field_index),
			  tr("Invalid table field index: %1").arg(table_field_index),
			  return ret);
	//QVariant v = val;
	//if(isNullReportedAsString() && v.toString() == qfc::Utils::nullValueString())
	//	v = QVariant();
	qfu::TableRow &r = m_table.rowRef(row);
	r.setValue(table_field_index, val);
	ret = true;

	return ret;
}

bool TableModel::setValue(int row_ix, const QString &col_name, const QVariant &val)
{
	int col_ix = columnIndex(col_name);
	QF_ASSERT(col_ix >= 0,
			  tr("Cannot find column index for name: '%1'").arg(col_name),
			  return false);
	return setValue(row_ix, col_ix, val);
}

QVariant TableModel::value(int row_ix, int column_ix) const
{
	QVariant ret;
	int table_field_index = tableFieldIndex(column_ix);
	//if(table_field_index < 0)
	//	qfDebug() << "debug";
	QF_ASSERT(table_field_index >= 0,
			  tr("%2 Cannot find table field for column index: %1 (row index: %3)").arg(column_ix).arg(objectName()).arg(row_ix),
			  return ret);
	ret = m_table.row(row_ix).value(table_field_index);
	/// DO NOT foreget that SQL NULL values are represented by null QVariants of appropriate column type
	/// NULL String is represented by QVariant(QVariant::String) NOT by QVariant()
	//qfInfo() << row_ix << column_ix << "->" << ret.toString() << ret.typeName();
	return ret;
}

QVariant TableModel::value(int row_ix, const QString &col_name) const
{
	QVariant ret;
	int col_ix = columnIndex(col_name);
	if(col_ix < 0) {
		qfError() << tr("Cannot find column index for name: '%1'").arg(col_name);
		return ret;
	}
	return value(row_ix, col_ix);
}

QVariant TableModel::origValue(int row_ix, int column_ix) const
{
	QVariant ret;
	int table_field_index = tableFieldIndex(column_ix);
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find table field for column index: %1").arg(column_ix),
			  return ret);
	ret = m_table.row(row_ix).origValue(table_field_index);
	/// DO NOT foreget that SQL NULL values are represented by null QVariants of appropriate columnt type
	/// NULL String is represented by QVariant(QVariant::String) NOT by QVariant()
	return ret;
}

QVariant TableModel::origValue(int row_ix, const QString &col_name) const
{
	QVariant ret;
	int col_ix = columnIndex(col_name);
	QF_ASSERT(col_ix >= 0,
			  tr("Cannot find column index for name: '%1'").arg(col_name),
			  return ret);
	return origValue(row_ix, col_ix);
}

QVariantMap TableModel::values(int row_ix) const
{
	QVariantMap ret;
	for (int i=0; i<columnCount(); ++i) {
		auto cd = columnDefinition(i);
		QString key;
		qf::core::Utils::parseFieldName(cd.fieldName(), &key);
		ret[key] = value(row_ix, i);
	}
	return ret;
}

bool TableModel::isDirty(int row_ix, int column_ix) const
{
	bool ret = false;
	int table_field_index = tableFieldIndex(column_ix);
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find table field for column index: %1").arg(column_ix),
			  return ret);
	ret = m_table.row(row_ix).isDirty(table_field_index);
	/// DO NOT foreget that SQL NULL values are represented by null QVariants of appropriate columnt type
	/// NULL String is represented by QVariant(QVariant::String) NOT by QVariant()
	return ret;
}

bool TableModel::isDirty(int row_ix, const QString &col_name) const
{
	bool ret = false;
	int col_ix = columnIndex(col_name);
	QF_ASSERT(col_ix >= 0,
			  tr("Cannot find column index for name: '%1'").arg(col_name),
			  return ret);
	return isDirty(row_ix, col_ix);
}

bool TableModel::setDirty(int row, int column, bool d)
{
	QF_ASSERT(m_table.isValidRowIndex(row),
			  tr("Invalid table row: %1").arg(row),
			  return false);
	int table_field_index = tableFieldIndex(column);
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find table field index for column index: %1").arg(column),
			  return false);
	QF_ASSERT(m_table.isValidFieldIndex(table_field_index),
			  tr("Invalid table field index: %1").arg(table_field_index),
			  return false);
	qfu::TableRow &r = m_table.rowRef(row);
	r.setDirty(table_field_index, d);
	return true;
}

bool TableModel::setDirty(int row_ix, const QString &col_name, bool d)
{
	int col_ix = columnIndex(col_name);
	QF_ASSERT(col_ix >= 0,
			  tr("Cannot find column index for name: '%1'").arg(col_name),
			  return false);
	return setDirty(row_ix, col_ix, d);
}

qf::core::utils::TableRow TableModel::tableRow(int row_no) const
{
	return m_table.row(row_no);
}

void TableModel::createColumnsFromTableFields()
{
	clearColumns();
	Q_FOREACH(auto fld, m_table.fields()) {
		QString caption;
		qfc::Utils::parseFieldName(fld.name(), &caption);
		addColumn(fld.name(), caption);
	}
	m_autoColumns = true;
	emit columnsAutoGenerated();
}

void TableModel::fillColumnIndexes()
{
	for(int i=0; i<m_columns.count(); i++) {
		ColumnDefinition &c = m_columns[i];
		int ix = -1;
		if(!c.isVirtual()) {
			ix = m_table.fields().fieldIndex(c.fieldName());
			if(ix < 0)
				qfWarning() << "\tcolumn" << c.fieldName() << "not found in fields.";
		}
		c.setFieldIndex(ix);
	}
}

int TableModel::columnType(int column_index) const
{
	int ret = QVariant::Invalid;
	ColumnDefinition cd = m_columns.value(column_index);
	QF_ASSERT(!cd.isNull(),
			  tr("Invalid column index: %1").arg(column_index),
			  return ret);
	ret = cd.castType();
	if(ret == QVariant::Invalid) {
		if(cd.isVirtual()) {
			qfWarning() << "Virtual column shoul have cast type defined!";
		}
		else {
			qfu::Table::Field fld = tableField(column_index);
			QF_ASSERT(!fld.isNull(), tr("Invalid field for column index: %1").arg(column_index), return ret);
			ret = fld.type();
		}
	}
	//qfInfo() << cd.fieldName() << ret;
	return ret;
}

QVariant TableModel::rawValueToEdit(int column_index, const QVariant &val) const
{
	Q_UNUSED(column_index);
	return val;
}

QVariant TableModel::editValueToRaw(int column_index, const QVariant &val) const
{
	Q_UNUSED(column_index);
	return val;
}

int TableModel::columnIndex(const QString &column_name) const
{
	int ret = -1, i = 0;
	Q_FOREACH(auto cd, m_columns) {
		//qfInfo() << cd.fieldName() << "vs." << column_name;
		if(qfc::Utils::fieldNameEndsWith(cd.fieldName(), column_name)) {
			ret = i;
			break;
		}
		i++;
	}
	/*
	if(throw_exc && ret < 0) {
		QStringList sl;
		foreach(ColumnDefinition cd, columns()) sl << cd.fieldName();
		QString s = sl.join(", ");
		QString msg = tr("Column named '%1' not found in column list. Existing columns: [%2]").arg(column_name).arg(s);
		QF_EXCEPTION(msg);
	}
	*/
	return ret;
}

int TableModel::tableFieldIndex(int column_index) const
{
	int ret = -1;
	ColumnDefinition cd = m_columns.value(column_index);
	QF_ASSERT(!cd.isNull(),
			  tr("Invalid column index: %1").arg(column_index),
			  return ret);
	ret = cd.fieldIndex();
	return ret;
}

qf::core::utils::Table::Field TableModel::tableField(int column_index) const
{
	qfu::Table::Field ret;
	int table_field_index = tableFieldIndex(column_index);
	//if(table_field_index < 0) for (int i = 0; i < columnCount(); ++i) {
	//	qfInfo() << i << columnDefinition(i).fieldName() << columnDefinition(i).fieldIndex();
	//}
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find field index for column index: %1 of %2 columns").arg(column_index).arg(columnCount()),
			  return ret);
	ret = m_table.field(table_field_index);
	QF_CHECK(!ret.isNull(),
			 tr("Cannot find table field for column index: %1, table index: %2").arg(column_index).arg(table_field_index));
	return ret;
}

void TableModel::clearColumns()
{
	if(m_columns.isEmpty())
		return;
	beginRemoveColumns(QModelIndex(), 0, m_columns.count() - 1);
	m_columns.clear();
	m_autoColumns = false;
	endRemoveColumns();
}

TableModel::ColumnDefinition &TableModel::insertColumn(int before_ix, const QString &field_name, const QString &caption)
{
	ColumnDefinition cd = ColumnDefinition(field_name);
	cd.setCaption(caption);
	return insertColumn(before_ix, cd);
}

TableModel::ColumnDefinition &TableModel::insertColumn(int before_ix, const TableModel::ColumnDefinition &cd)
{
	if(before_ix < 0 || before_ix > m_columns.count())
		before_ix = m_columns.count();
	m_columns.insert(before_ix, cd);
	ColumnDefinition &c = m_columns[before_ix];
	return c;
}

void TableModel::setColumn(int ix, const TableModel::ColumnDefinition &cd)
{
	QF_ASSERT(ix >= 0 && ix < columnCount(), "invalid column index", return);
	m_columns[ix] = cd;
}

TableModel::ColumnDefinition TableModel::removeColumn(int ix)
{
	qfError() << Q_FUNC_INFO << ix << "NIY";
	TableModel::ColumnDefinition ret;
	return ret;
}

void TableModel::setTable(const qf::core::utils::Table &t)
{
	beginResetModel();
	m_table = t;
	endResetModel();
}

int TableModel::insertTableRow(int before_row)
{
	qfLogFuncFrame() << "before_row:" << before_row << "row cnt:" << rowCount();
	if(before_row < 0 || before_row > rowCount())
		before_row = rowCount();
	m_table.insertRow(before_row);
	qfDebug() << "\t row cnt:" << rowCount();
	return before_row;
}

bool TableModel::insertRows(int row_ix, int count, const QModelIndex &parent)
{
	qfLogFuncFrame() << "row:" << row_ix << "count:" << count;
	if(count < 0)
		return false;
	if(row_ix < 0)
		row_ix = 0;
	beginInsertRows(parent, row_ix, row_ix + count - 1);
	bool ok = true;
	for(int i=0; i<count; i++) {
		ok = insertTableRow(row_ix + i);
		if(!ok)
			break;
	}
	endInsertRows();
	return ok;
}

bool TableModel::removeTableRow(int row_ix, bool throw_exc)
{
	Q_UNUSED(throw_exc);
	bool ok = m_table.removeRow(row_ix);
	return ok;
}

bool TableModel::removeRows(int row_ix, int count, bool throw_exc)
{
	qfLogFuncFrame() << "row:" << row_ix << "count:" << count;
	if(count < 0)
		return false;
	bool ok = true;
	beginRemoveRows(QModelIndex(), row_ix, row_ix + count - 1);
	for(int i=0; i<count; i++) {
		ok = removeTableRow(row_ix, throw_exc);
		if(!ok)
			break;
	}
	endRemoveRows();
	return ok;
}

bool TableModel::removeRowNoOverload(int row_ix, bool throw_exc)
{
	qfLogFuncFrame() << "row:" << row_ix;
	beginRemoveRows(QModelIndex(), row_ix, row_ix);
	bool ok = TableModel::removeTableRow(row_ix, throw_exc);
	endRemoveRows();
	return ok;
}

qfu::TreeTable TableModel::toTreeTable(const QString& table_name, const QVariantList& _exported_columns, const TreeTableExportOptions &opts) const
{
	qfu::TreeTable ret(table_name);
	QVariantList exported_columns = _exported_columns;
	if(exported_columns.isEmpty()) {
		for(int ix=0; ix<m_table.columnCount(); ix++) {
			QVariantMap col;
			col[QStringLiteral("index")] = ix;
			exported_columns << col;
		}
	}
	for(int i=0; i<exported_columns.count(); i++) {
		QVariantMap col = exported_columns[i].toMap();
		QString cap = col.value("caption").toString();
		int ix = col.value("index").toInt();
		qfu::TreeTableColumn tt_col;
		if(col.value("origin") == QLatin1String("model")) {
			QVariant::Type t = (QVariant::Type)headerData(ix, Qt::Horizontal, FieldTypeRole).toInt();
			tt_col = ret.appendColumn(headerData(ix, Qt::Horizontal, FieldNameRole).toString(), t, cap);
		}
		else {
			QVariant::Type t = m_table.field(ix).type();
			tt_col = ret.appendColumn(m_table.field(ix).name(), t, cap);
		}
		tt_col.setWidth(col.value("width").toString());
	}

	/// export data
	{
		bool raw_values = opts.isExportRawValues();
		qfu::SValue srows;
		for(int i=0; i<rowCount(); i++) {
			QVariantList srow_lst;
			qfu::Table::Row tbl_row = m_table.row(i);
			//if(cnt) if(n++ % progress_step) emit progressValue(1.*n/cnt, tr("Probiha export"));
			for(int j=0; j<exported_columns.count(); j++) {
				QVariantMap col = exported_columns[j].toMap();
				QVariant val;
				int ix = col.value("index").toInt();
				if(col.value("origin") == QLatin1String("model")) {
					if(raw_values) {
						val = value(i, ix);
						//qfWarning() << val.typeName() << "val:" << val.toString();
					}
					else {
						QModelIndex mix = index(i, ix);
						val = data(mix);
					}
				}
				else {
					val = tbl_row.value(ix);
				}
				srow_lst << val;
			}
			srows[i] = srow_lst;
		}
		ret[qfu::TreeTable::KEY_ROWS] = srows.value();
	}
	return ret;
}

QVariant TableModel::toTreeTableData(const QString &table_name, const QVariantList &exported_columns) const
{
	qfu::TreeTable tt = toTreeTable(table_name, exported_columns);
	//qfInfo() << tt.toString();
	QVariant ret = tt.toVariant();
	return ret;
}

TableModel::ColumnDefinition TableModel::columnDefinition(int ix) const
{
	return m_columns.value(ix);
}
