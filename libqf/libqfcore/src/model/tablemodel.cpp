#include "tablemodel.h"
#include "../core/log.h"
#include "../core/assert.h"
#include "../core/utils.h"

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

//=========================================
//        TableModel
//=========================================
QString TableModel::m_defaultTimeFormat = QStringLiteral("hh:mm:ss");
QString TableModel::m_defaultDateFormat = QStringLiteral("yyyy-MM-dd");
QString TableModel::m_defaultDateTimeFormat = QStringLiteral("yyyy-MM-ddThh:mm:ss");

TableModel::TableModel(QObject *parent) :
	Super(parent), m_nullReportedAsString(true)
{
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
			qfu::Table::Field field = tableField(index.column());
			if(!field.isNull()) {
				can_edit = can_edit && field.canUpdate();
				QVariant::Type type = columnType(index.column());
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
		if(can_edit)
			flags |= Qt::ItemIsEditable;
		else
			flags &= ~Qt::ItemIsEditable;
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
	else if(role == Qt::DisplayRole) {
		//qfDebug() << QF_FUNC_NAME;
		//qfDebug() << "\trow:" << ix.row() << "col:" << ix.column() << "role:" << role;
		ColumnDefinition cd = m_columns.value(index.column());
		if(cd.isNull()) {
			return QString("!%1").arg(index.column());
		}
		qfu::Table::Field field = tableField(index.column());
		if(field.isNull())
			return QString("!%1").arg(cd.fieldName());

		if(data(index, ValueIsNullRole).toBool()) {
			if(isNullReportedAsString())
				return QStringLiteral("null");
			return QVariant();
		}
		ret = data(index, RawValueRole);
		QVariant::Type type = columnType(index.column());
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
			/*
			else if(type == QVariant::Int) {
				format = intFormat();
			}
			else if(type == QVariant::Double) {
				format = doubleFormat();
			}
			*/
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
			/*
			else if(type == QVariant::Int) {
				ret = QFString::number(ret.toInt(), format);
			}
			else if(type == QVariant::Double) {
				ret = QFString::number(ret.toDouble(), format);
			}
			*/
		}
	}
	else if(role == Qt::EditRole) {
		ret = data(index, RawValueRole);
	}
	else if (role == ValueIsNullRole) {
		ret = data(index, RawValueRole);
		return ret.isNull();
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
		QVariant::Type type = columnType(index.column());
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
		QVariant::Type type = columnType(index.column());
		if(type == QVariant::Bool) {
			//qfInfo() << "BOOL";
			return (data(index, Qt::EditRole).toBool()? Qt::Checked: Qt::Unchecked);
		}
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
			QVariant::Type type = columnType(section);
			return QVariant((int)type);
		}
		/*
		else if (role == FieldIsNullableRole) {
			qfu::Table::Field field = tableField(section);
			/// drivery nastavuji requiredStatus() jako !nullable
			return QVariant(field.isNullable());
		}
		*/
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
	qfLogFuncFrame() << value.toString() << "role:" << role;
	bool ret = false;
	if(!index.isValid())
		return ret;
	if(role == Qt::EditRole) {
		ret = setValue(index.row(), index.column(), value);
		if(ret)
			emit dataChanged(index, index);
	}
	else if(role == Qt::CheckStateRole) {
		bool chk_val = (value.toInt() == Qt::Unchecked)? 0: 1;
		ret = setValue(index.row(), index.column(), chk_val);
		if(ret)
			emit dataChanged(index, index);
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
	QF_ASSERT(table_field_index >= 0,
			  tr("Cannot find table field for column index: %1").arg(column_ix),
			  return ret);
	ret = m_table.row(row_ix).value(table_field_index);
	return ret;
}

QVariant TableModel::value(int row_ix, const QString &col_name) const
{
	QVariant ret;
	int col_ix = columnIndex(col_name);
	QF_ASSERT(col_ix >= 0,
			  tr("Cannot find column index for name: '%1'").arg(col_name),
			  return ret);
	return value(row_ix, col_ix);
}

void TableModel::createColumnsFromTableFields()
{
	m_columns.clear();
	for(auto fld : m_table.fields()) {
		QString caption;
		qfc::Utils::parseFieldName(fld.name(), &caption);
		addColumn(fld.name(), caption);
	}
}

void TableModel::fillColumnIndexes()
{
	for(int i=0; i<m_columns.count(); i++) {
		ColumnDefinition &c = m_columns[i];
		int ix = m_table.fields().fieldIndex(c.fieldName());
		if(ix < 0)
			qfWarning() << "\tcolumn" << c.fieldName() << "not found in fields.";
		c.setFieldIndex(ix);
	}
}

QVariant::Type TableModel::columnType(int column_index) const
{
	QVariant::Type ret = QVariant::Invalid;
	ColumnDefinition cd = m_columns.value(column_index);
	QF_ASSERT(!cd.isNull(),
			  tr("Invalid column index: %1").arg(column_index),
			  return ret);
	ret = cd.castType();
	if(ret == QVariant::Invalid) {
		qfu::Table::Field fld = tableField(column_index);
		QF_ASSERT(!fld.isNull(),
				  tr("Invalid field for column index: %1").arg(column_index),
				  return ret);
		ret = fld.type();
	}
	return ret;
}

int TableModel::columnIndex(const QString &column_name) const
{
	int ret = -1, i = 0;
	for(auto cd : m_columns) {
		//qfTrash() << "\ttrying:" << cd.fieldName();
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
	QF_ASSERT(table_field_index >= 0,
			  tr("Invalid column index: %1").arg(column_index),
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

