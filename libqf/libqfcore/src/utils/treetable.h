//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_CORE_UTILS_TREETABLE_H
#define QF_CORE_UTILS_TREETABLE_H

#include "../core/coreglobal.h"
#include "svalue.h"

#include <QVariantMap>
#include <QVariantList>

namespace qf {
namespace core {
namespace utils {

/// helper table used for repor generator mainly
/// should be replaced by Table in future
class QFCORE_DECL_EXPORT TreeTableColumn : public SValue
{
public:
	QString name() const;
	void setName(const QString &n);
	QString typeName() const;
	void setTypeName(const QString &tn);
	QVariant::Type type() const;
	void setType(QVariant::Type t);
	QString width() const;
	void setWidth(const QString &w);
	QString header() const;
	QString footer() const;
	QString hAlignment() const;
public:
	TreeTableColumn() : SValue() {}
	TreeTableColumn(const SValue &sv) : SValue(sv) {}
	//virtual ~TreeTableRow();
};

//! TODO: write class documentation.
class QFCORE_DECL_EXPORT TreeTableColumns : public SValue
{
protected:
public:
	int indexOf(const QString &col_name) const;
	TreeTableColumn column(int ix) const {return SValue(property(ix));}
	TreeTableColumn column(const QString &col_name) const {return column(indexOf(col_name));}
	//SValue column(const QString &col_name) const {return value(indexOf(col_name)).toMap();}
public:
	TreeTableColumns() : SValue() {}
	TreeTableColumns(const QVariant &v);
	//virtual ~TreeTableRow();
};

//! TODO: write class documentation.
class QFCORE_DECL_EXPORT TreeTableRow// : public SValue
{
protected:
	TreeTableColumns f_columns;
	SValue f_row;
protected:
	void convertListToMap();
	QVariant value_helper(int ix, bool &found) const;
	QVariant value_helper(const QString &col_or_key_name, bool &found) const;
public:
	bool isNull() const {return !f_row.isValid();}
	TreeTableColumns columns() const {return f_columns;}

	SValue rowData() const {return f_row;}

	QVariant value(int col) const;
	QVariant value(int col, const QVariant &default_val) const;
	QVariant value(const QString &col_or_key_name) const;
	QVariant value(const QString &col_or_key_name, const QVariant &default_val) const;
	void setValue(int col, const QVariant &val);
	void setValue(const QString &col_or_key_name, const QVariant &val);

	SValue keyvals() const;
	SValue keyvalsRef();

	int tablesCount() const;
	SValue table(int ix) const;
	SValue table(const QString &name = QString()) const;
	void appendTable(const SValue &t);
public:
	TreeTableRow() {}
	TreeTableRow(const TreeTableColumns &columns, const SValue &row_data);
	virtual ~TreeTableRow();
};

//! TODO: write class documentation.
class QFCORE_DECL_EXPORT TreeTable : public SValue
{
public:
	static const QString KEY_TYPE;
	static const QString KEY_META;
	static const QString KEY_NAME;
	static const QString KEY_COLUMNS;
	static const QString KEY_ROWS;
	static const QString KEY_ROW;
	static const QString KEY_KEYVALS;
	static const QString KEY_TABLES;
public:
	TreeTable();
	TreeTable(const QString &table_name) {setTableName(table_name);}
	TreeTable(const SValue &sv) : SValue(sv) {}
	virtual ~TreeTable();
protected:
	SValue rows() const {return SValue(property(KEY_ROWS));}
	//void setRows(const QVariantList &new_rows) {f_data[KEY_ROWS] = new_rows;}
	//void setColumns(const TreeTableColumns &cols) {f_data[KEY_COLUMNS] = cols;}
public:
	bool isNull() const {return !isValid();}
	QString name() const {return property2(KEY_META).property2(KEY_NAME).value().toString();}
	void setName(const QString &n) {(*this)[KEY_META][KEY_NAME] = n;}
	TreeTableColumns columns() const {return TreeTableColumns(property(KEY_COLUMNS));}
	int columnCount() const {return columns().count();}
	int rowCount() const {return rows().count();}
	TreeTableRow row(int ix) const {return TreeTableRow(columns(), rows().property(ix));}
	TreeTableRow appendRow();
	void removeRow(int ix);

	void setColumnHeader(const QString &col_name, const QString &header);
	QString columnHeader(const QString &col_name) const;
	void setColumnFooter(const QString &col_name, const QString &footer);
	QString columnFooter(const QString &col_name) const;
	/// pokud je tabulka v reportu generovana z dat, je sloupec takto zarovnan
	void setColumnAlignment(const QString &col_name, Qt::Alignment alignment);
	void setColumnAlignment(int col_no, Qt::Alignment alignment);
	QString columnHAlignment(const QString &col_name) const;

	//! pokud je \a caption == QString(), vezme se to za posledni teckou z \a name nebo cely \a name , kdyz tam neni tecka.
	TreeTableColumn appendColumn(const QString &name, QVariant::Type type = QVariant::String, const QString &caption = QString());

	//bool isNull() const {return columnCount() == 0;}

	//! @param key_ends_with if true key name is compared using function QFSql::endsWith().
	/// pokud se vyskytuje agregacni funkce, musi byt okolo jmena fieldu, napr. SUM(cena)
	QVariant value(const QString &key_name, const QVariant &default_val = QVariant(), bool key_ends_with = true) const;
	void setValue(const QString &key_name, const QVariant &val);

	QVariant sum(const QString &col_name) const;
	QVariant sum(int col_index) const;
	/// pokud by musel delit 0, vraci QVariant().
	QVariant average(const QString &col_name) const;
	QVariant average(int col_index) const;

	QString tableName() const;
	void setTableName(const QString &name);

	SValue keyvals();
	const SValue keyvals() const;

	/// hleda tabulku v detech
	/// cesta ma format n/table_name1/n/table_name2/... , kde n je cislo radku, ve kterem se ma tabulka hledat
	TreeTable cd(const QString &path) const;

	/// options:
	/// lineSeparator: cim se budou oddelovat radky, default je '\n'
	/// lineIndent: cim se budou uvozovat vnorene urovne radku, default jsou dve mezery
	/// lineOffset: cim se budou uvozovat vschny radky, default je QString()
	QString toHtml(const QVariantMap &opts = QVariantMap()) const;
};

}}}

Q_DECLARE_METATYPE(qf::core::utils::TreeTable)

#endif // TreeTable_H

