//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_CORE_UTILS_TREETABLE_H
#define QF_CORE_UTILS_TREETABLE_H

#include "../core/coreglobal.h"

#include <QVariantMap>
#include <QVariantList>
#include <QJsonDocument>

namespace qf {
namespace core {
namespace utils {
#if 0
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

#endif

class TreeTable;
class QFCORE_DECL_EXPORT TreeTableRow
{
public:
	TreeTableRow() {}
	TreeTableRow(const QVariant &columns, const QVariant &row_data)
		: m_columns(columns.toList())
		, m_row(row_data)
	{}

	bool isValid() const {return m_row.isValid();}
	int columnCount() const {return m_columns.count();}
	int columnIndex(const QString &col_name) const;

	QVariant value(int col_ix) const;
	QVariant value(const QString &col_or_key_name) const;

	int tablesCount() const;
	TreeTable table(int ix = 0) const;
	TreeTable table(const QString &table_name) const;
private:
	QVariant retypeVariant(int col_ix, const QVariant &v) const;
private:
	QVariantList m_columns;
	QVariant m_row;
};

class QFCORE_DECL_EXPORT TreeTable
{
public:
	static const QString KEY_NAME;
	static const QString KEY_TYPE; // col type
	static const QString KEY_HEADER;
	static const QString KEY_FOOTER;
	static const QString KEY_HALIGN;
	static const QString KEY_VALIGN;
	static const QString KEY_WIDTH;

	static const QString KEY_META;

	static const QString KEY_COLUMNS;
	static const QString KEY_ROWS;

	static const QString KEY_ROW;
	static const QString KEY_KEYVALS;
	static const QString KEY_TABLES;
public:
	TreeTable() {}
	TreeTable(const QVariant &value, const QString &table_name) : m_values(value) {setName(table_name);}
	TreeTable(const QString &table_name) {setName(table_name);}
	TreeTable(const QVariant &value) : m_values(value) {}
	//virtual ~TreeTable();
protected:
	//SValue rows() const {return SValue(property(KEY_ROWS));}
	//void setRows(const QVariantList &new_rows) {f_data[KEY_ROWS] = new_rows;}
	//void setColumns(const TreeTableColumns &cols) {f_data[KEY_COLUMNS] = cols;}
public:
	bool isValid() const {return m_values.isValid();}
	QString name() const {return m_values.toMap().value(KEY_META).toMap().value(KEY_NAME).toString();}
	void setName(const QString &n);

	int columnCount() const {return m_values.toMap().value(KEY_COLUMNS).toList().count();}
	int rowCount() const {return m_values.toMap().value(KEY_ROWS).toList().count();}
	//TreeTableRow row(int ix) const {return TreeTableRow(columns(), rows().property(ix));}
	int insertRow(int ix, const QVariantList &vals = QVariantList());
	int appendRow(const QVariantList &vals = QVariantList());
	void removeRow(int ix);

	void setColumnWidth(int col_ix, const QVariant &width);
	void setColumnWidth(const QString &col_name, const QVariant &width);
	QVariant columnWidth(int col_ix) const;
	QVariant columnWidth(const QString &col_name) const;

	void setColumnHeader(const QString &col_name, const QString &header);
	QString columnHeader(int col_ix) const;
	QString columnHeader(const QString &col_name) const;
	void setColumnFooter(const QString &col_name, const QString &footer);
	QString columnFooter(const QString &col_name) const;
	/// pokud je tabulka v reportu generovana z dat, je sloupec takto zarovnan
	void setColumnAlignment(const QString &col_name, Qt::Alignment alignment);
	void setColumnAlignment(int col_no, Qt::Alignment alignment);

	int columnIndex(const QString &col_name) const;
	QString columnName(int col_no) const { return columns().value(col_no).toMap().value(KEY_NAME).toString(); }
	int columnType(int col_no) const;
	void setColumnType(int col_no, int meta_type_id);

	//! pokud je \a caption == QString(), vezme se to za posledni teckou z \a name nebo cely \a name , kdyz tam neni tecka.
	void appendColumn(const QString &name, QVariant::Type type = QVariant::String, const QString &caption = QString());

	QVariant toVariant() const {return m_values;}
	TreeTableRow row(int row_ix) const;

	//! @param key_ends_with if true key name is compared using function QFSql::endsWith().
	/// pokud se vyskytuje agregacni funkce, musi byt okolo jmena fieldu, napr. SUM(cena)
	QVariant value(const QString &key_name, const QVariant &default_val = QVariant(), bool key_ends_with = true) const;
	void setValue(const QString &key_name, const QVariant &val);

	//QVariant value(int row_ix, const QString &col_or_key_name) const;
	//QVariant value(int row_ix, int col_ix) const;

	void setValue(int row_ix, int col_ix, const QVariant &val);
	void setValue(int row_ix, const QString &col_or_key_name, const QVariant &val);

	QVariant sum(const QString &col_name) const;
	QVariant sum(int col_index) const;
	/// pokud by musel delit 0, vraci QVariant().
	QVariant average(const QString &col_name) const;
	QVariant average(int col_index) const;

	/// hleda tabulku v detech
	/// cesta ma format n/table_name1/n/table_name2/... , kde n je cislo radku, ve kterem se ma tabulka hledat
	TreeTable cd(const QString &path) const;

	int tablesCount(int row_ix) const;
	TreeTable table(int row_ix, int table_ix = 0) const;
	TreeTable table(int row_ix, const QString &table_name) const;
	void appendTable(int row_ix, const TreeTable &t);

	QVariantMap keyvals(int row_ix) const;

	/// options:
	/// lineSeparator: cim se budou oddelovat radky, default je '\n'
	/// lineIndent: cim se budou uvozovat vnorene urovne radku, default jsou dve mezery
	/// lineOffset: cim se budou uvozovat vschny radky, default je QString()
	QString toHtml(const QVariantMap &opts = QVariantMap()) const;

	QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
	QString toString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
private:
	//QVariant retypeVariant(int col_ix, const QVariant &v) const;

	QVariantMap valueMap() const { return m_values.toMap(); }
	QVariantMap meta() const { return m_values.toMap().value(KEY_META).toMap(); }

	QVariantMap keyvals() const { return m_values.toMap().value(KEY_KEYVALS).toMap(); }
	void setKeyvals(const QVariantMap &keyvals);

	QVariantList columns() const { return m_values.toMap().value(KEY_COLUMNS).toList(); }
	void setColumns(const QVariantList &columns);

	QVariantList rows() const { return m_values.toMap().value(KEY_ROWS).toList(); }
	void setRows(const QVariantList &rows);
private:
	QVariant m_values;
};

}}}

Q_DECLARE_METATYPE(qf::core::utils::TreeTable)

#endif // TreeTable_H

