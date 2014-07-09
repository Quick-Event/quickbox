#include "tablemodel.h"
#include "../core/log.h"

#include <QTime>
#include <QColor>

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
		QVariant::Type type = field.type();
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
				format = defaultDateFormat();
			}
			else if(type == QVariant::Time) {
				format = defaultTimeFormat();
				//qfInfo() << "format" << format;
			}
			else if(type == QVariant::DateTime) {
				format = defaultDateTimeFormat();
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
		const ColumnDefinition &cd = m_columns.value(index.column());
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
		qfu::Table::Field field = tableField(index.column());
		QVariant::Type type = field.type();
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
		qfu::Table::Field field = tableField(index.column());
		QVariant::Type type = field.type();
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

qf::core::utils::Table::Field TableModel::tableField(int column_index) const
{
	qfu::Table::Field field;
	ColumnDefinition cd = m_columns.value(column_index);
	if(!cd.isNull()) {
		int table_field_index = cd.fieldIndex();
		field = m_table.field(table_field_index);
		if(field.isNull())
			qfWarning() << "Cannot find table field for column index:" << column_index;
	}
	return field;
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
	qfLogFuncFrame() << field_name << "before_ix:" << before_ix;
	if(before_ix < 0 || before_ix > m_columns.count())
		before_ix = m_columns.count();
	m_columns.insert(before_ix, ColumnDefinition(field_name));
	ColumnDefinition &c = m_columns[before_ix];
	c.setCaption(caption);
	return c;
}

