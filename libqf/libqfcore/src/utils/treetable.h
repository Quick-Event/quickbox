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

class QFCORE_DECL_EXPORT TreeTableColumn
{
public:
	TreeTableColumn() {}
	TreeTableColumn(const QVariantMap &mv) : m_values(mv) {}

	QVariantMap values() const {return m_values;}

	QString name() const;
	void setName(const QString &n);

	QString typeName() const;
	void setTypeName(const QString &tn);
	int type() const;
	void setType(int t);

	QVariant width() const;
	void setWidth(const QVariant &w);

	QString header() const;
	void setHeader(const QString &s);
	QString footer() const;

	QString halign() const;
	Qt::Alignment alignment() const;
private:
	void setKeyValue(const QString &key, const QVariant &val);
private:
	QVariantMap m_values;
};

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

	QVariant row() const { return  m_row; }

	QVariant value(int col_ix) const;
	QVariant value(const QString &col_or_key_name) const;

	void setValue(int col_ix, const QVariant &val);
	void setValue(const QString &col_or_key_name, const QVariant &val);

	int tablesCount() const;
	TreeTable table(int ix = 0) const;
	TreeTable table(const QString &table_name) const;
	void appendTable(const TreeTable &t);
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
public:
	bool isValid() const {return m_values.isValid();}
	QString name() const {return m_values.toMap().value(KEY_META).toMap().value(KEY_NAME).toString();}
	void setName(const QString &n);

	static int columnIndex(const QVariantList &cols, const QString &col_name);
	int columnIndex(const QString &col_name) const {return columnIndex(columns(), col_name);}
	int columnCount() const {return m_values.toMap().value(KEY_COLUMNS).toList().count();}
	void appendColumn(const QString &name, QMetaType type = QMetaType(QMetaType::QString), const QString &caption = QString());
	void appendColumn(const TreeTableColumn &c);

	TreeTableColumn column(int col_ix) const;
	TreeTableColumn column(const QString &col_name) const {return column(columnIndex(col_name));}
	void setColumn(int col_ix, const TreeTableColumn &ttc);

	int rowCount() const {return m_values.toMap().value(KEY_ROWS).toList().count();}
	//TreeTableRow row(int ix) const {return TreeTableRow(columns(), rows().property(ix));}
	int insertRow(int ix, const QVariantList &vals = QVariantList());
	int appendRow(const QVariantList &vals = QVariantList());
	void removeRow(int ix);

	TreeTableRow row(int row_ix) const;
	void setRow(int row_ix, const TreeTableRow &ttr);

	//! @param key_ends_with if true key name is compared using function QFSql::endsWith().
	/// pokud se vyskytuje agregacni funkce, musi byt okolo jmena fieldu, napr. SUM(cena)
	QVariant value(const QString &key_name, const QVariant &default_val = QVariant(), bool key_ends_with = true) const;
	void setValue(const QString &key_name, const QVariant &val);

	void appendTable(int row_ix, const TreeTable &t);

	QVariant toVariant() const {return m_values;}

	QVariant sum(const QString &col_name) const;
	QVariant sum(int col_index) const;
	/// pokud by musel delit 0, vraci QVariant().
	QVariant average(const QString &col_name) const;
	QVariant average(int col_index) const;

	/// hleda tabulku v detech
	/// cesta ma format n/table_name1/n/table_name2/... , kde n je cislo radku, ve kterem se ma tabulka hledat
	TreeTable cd(const QString &path) const;

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

