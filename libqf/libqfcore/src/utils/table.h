#ifndef QF_CORE_UTILS_TABLE_H
#define QF_CORE_UTILS_TABLE_H

#include "../core/coreglobal.h"
#include "../core/utils.h"
#include "svalue.h"

#include <QString>
#include <QVariantMap>
#include <QRegExp>
#include <QCollator>
#include <QVector>
#include <QBitArray>

class QDomElement;
class QDomDocument;

namespace qf {
namespace core {
namespace utils {

class TableRow;
typedef QList<TableRow> RowList;

class QFCORE_DECL_EXPORT Table
{
	//Q_DECLARE_TR_FUNCTIONS(qf::core::utils::Table);
public:
	enum CleanupDataOption {ClearFieldsRows = 1, ClearRows};
	class QFCORE_DECL_EXPORT TextImportOptions : public QVariantMap
	{
	public:
		QF_OPTION_FIELD2_RW(QString, f, setF, ieldSeparator, ",")
		QF_OPTION_FIELD2_RW(QString, f, setF, ieldQuotes, "\"")
		QF_OPTION_FIELD2_RW(bool, isI, setI, mportColumnNames, true)
		QF_OPTION_FIELD2_RW(bool, isI, setI, mportAppend, false)
		QF_OPTION_FIELD2_RW(int, i, setI, gnoreFirstLinesCount, 0)
		QF_OPTION_FIELD2_RW(bool, isT, setT, rimValues, true)
	};
	class QFCORE_DECL_EXPORT TextExportOptions : public QVariantMap
	{
	public:
		enum FieldQuoting {Never=1, IfNecessary, Always};
	public:
		QF_OPTION_FIELD2_RW(QString, f, setF, ieldSeparator, ",")
		QF_OPTION_FIELD2_RW(QString, f, setF, ieldQuotes, "\"")
		QF_OPTION_FIELD2_RW(bool, isE, setE, xportColumnNames, true)
		QF_OPTION_FIELD2_RW(bool, isF, setF, ullColumnNames, true)
		QF_OPTION_FIELD2_RW(bool, isU, setU, seColumnCaptions, true)

		QVariantMap columnCaptions() const {return value("columnCaptions").toMap();}
		TextExportOptions& setColumnCaptions(const QVariantMap &cc) {this->operator[]("columnCaptions") = cc; return *this;}

		FieldQuoting fieldQuotingPolicy() const {return (FieldQuoting)(value("fieldQuotingPolicy", IfNecessary).toInt());}
		TextExportOptions& setFieldQuotingPolicy(FieldQuoting fq) {this->operator[]("fieldQuotingPolicy") = fq; return *this;}

		//QF_OPTION_FIELD2_RW(FieldQuoting, f, setF, ieldQuotingPolicy, IfNecessary)
		QF_OPTION_FIELD_RW(QString, c, setC, odecName)
		QF_OPTION_FIELD2_RW(int, f, setF, romLine, 0)
		QF_OPTION_FIELD2_RW(int, t, setT, oLine, -1)
		QF_OPTION_FIELD_RW(QString, n, setN, ote)
		/// pri exportu do XLS se vlozi tabulka slozena z radku XLS prologu
		QF_OPTION_FIELD_RW(QVariantList, x, setX, lsProlog)
		public:
			TextExportOptions(const QVariantMap &m = QVariantMap()) : QVariantMap(m) {}
	};
public:
	//! Sorting information fo one field.
	struct QFCORE_DECL_EXPORT SortDef
	{
		static const bool Ascending = true;
		static const bool CaseSensitive = true;
		static const bool Ascii7Bit = true;
		int fieldIndex; //!< index of field to sort.
		bool ascending;
		bool caseSensitive;
		bool ascii7bit; //!< convert values to ASCII 7 bit for before sorting.

		bool isValid() const {return fieldIndex >= -1;}
		SortDef(int ix = -2, bool asc = Ascending, bool cs = CaseSensitive, bool _ascii7bit = !Ascii7Bit)
			: fieldIndex(ix), ascending(asc), caseSensitive(cs), ascii7bit(_ascii7bit) {}
	};
	typedef QList<SortDef> SortDefList;
public:
	class QFCORE_DECL_EXPORT LessThan
	{
	public:
		LessThan(const Table &t);
		virtual ~LessThan() {}
	public:
		inline bool operator()(int r1, int r2) const { return lessThan(r1, r2); }
		inline bool operator()(int r, const QVariant &v) const { return lessThan_helper(r, v, false); }
		inline bool operator()(const QVariant &v, int r) const { return lessThan_helper(r, v, true); }
	protected:
		bool lessThan(int r1, int r2) const;
		bool lessThan_helper(int _row, const QVariant &v, bool switch_params) const;
		virtual int cmp(const QVariant &l, const QVariant &r, const Table::SortDef &sd) const;
	protected:
		const Table &table;
		Table::SortDefList sortedFields;
		QCollator sortCollator;
	};
protected:
	typedef QVector<int> RowIndexList;
public:
	class QFCORE_DECL_EXPORT Field
	{
	private:
		class SharedDummyHelper {};
		class Data : public QSharedData
		{
		public:
			QVariant::Type type;
			QString name;
			Data() : type(QVariant::Invalid) {}
			Data(const QString &name, QVariant::Type t) : type(t), name(name) {}
		};
		QSharedDataPointer<Data> d;

		Field(SharedDummyHelper); /// null row constructor
		static const Field& sharedNull();
	public:
		Field();
		Field(const QString &name, QVariant::Type t);

		bool isNull() const {return d == sharedNull().d;}

		QF_SHARED_CLASS_FIELD_RW(QVariant::Type, t, setT, ype)
		QF_SHARED_CLASS_FIELD_RW(QString, n, setN, ame)
	};
	class QFCORE_DECL_EXPORT FieldList : public QList<Field>
	{
	public:
		/**
			finds index in field list, containing column \a field_name
			@param field_name fieldname in form [tablename.]fieldname
			@return field index or value lower than zero
			 */
		int fieldIndex(const QString &field_name, bool throw_exc = false) const;
		bool isValidFieldIndex(int fld_ix) const;
	};
public:
	class QFCORE_DECL_EXPORT TableProperties
	{
	private:
		class Data : public QSharedData
		{
		public:
			FieldList fields;
			SortDefList sortedFields; //!< Each sorted field has one entry in this list.
		public:
			Data() {}
			~Data() {}
		};
		QSharedDataPointer<Data> d;
		class SharedDummyHelper {};
		TableProperties(SharedDummyHelper);
	public:
		static const TableProperties& sharedNull();
		bool isNull() const {return d == sharedNull().d;}

		FieldList& fieldsRef() {return d->fields;}
		const FieldList& fields() const {return d->fields;}

		const SortDefList& sortDefinition() const {return d->sortedFields;}
		SortDefList& sortDefinitionRef() {return d->sortedFields;}
		void setSortDefinition(const SortDefList& sdl) {d->sortedFields = sdl;}

		bool operator==(const TableProperties &other) const {return d == other.d;}
	public:
		TableProperties();
	};
private:
	class Data : public QSharedData
	{
	public:
		//! Properties of table usefull for TableRow (fields, sorting info etc.). Each row has implicitly shared copy of this struct.
		TableProperties tableProperties;
		RowList rows; ///< copy of ResultSet
		//QStringList tableids; ///< list of tablenames in active query
		RowIndexList rowIndex; ///< rows are accessed through this index (sorting, filtering, ...)
		//int currentRow; ///< index of current row in \a index
		SortDefList sortedFields;
		QCollator sortCollator;
	public:
		Data();
		~Data() { }
	};
	class SharedDummyHelper {};
	Table(SharedDummyHelper); /// null row constructor
	static const Table& sharedNull();
	QSharedDataPointer<Data> d;
public:
	bool isNull() const {return d == sharedNull().d;}
	const TableProperties& tableProperties() const {return d->tableProperties;}
	const FieldList& fields() const {return tableProperties().fields();}
	//const RowList& rows() const {return d->rows;}

	QVariant sumValue(int field_ix) const;
	QVariant sumValue(const QString &field_name) const
	{
		return sumValue(fields().fieldIndex(field_name));
	}
private:
	const RowList& rows() const {return d->rows;}
	RowList& rowsRef() {return d->rows;}
	//! clears all rows, if \a fields_options tells what else will be cleared.
	void cleanupData(CleanupDataOption fields_options);
	int rowNumberToRowIndex(int rowno) const;
	void createRowIndex();
	const RowIndexList& rowIndex() const {return d->rowIndex;}
	RowIndexList& rowIndexRef() {return d->rowIndex;}
public:
	TableProperties& tablePropertiesRef() {return d->tableProperties;}
	FieldList& fieldsRef() {return tablePropertiesRef().fieldsRef();}
public:
	Field& fieldRef(int fld_ix);
	Field& fieldRef(const QString& field_name)
	{
		return fieldRef(fields().fieldIndex(field_name, true));
	}
	Field field(int fld_ix, bool throw_exc = false) const;
	Field field(const QString& field_name, bool throw_exc = false) const
	{
		return field(fields().fieldIndex(field_name, throw_exc), throw_exc);
	}

	TableRow& rowRef(int rowno);
	//! \sa rowRef(int)
	TableRow row(int i, bool throw_exc = false) const;
	/// vraci posledni radek, pokud neni a je throw_exc vrha exception, jinak vraci null row.
	TableRow lastRow(bool throw_exc = false) const;
	//static TableRow nullRow() {return TableRow::sharedNull();}
public:
	//! Clear all except column definitions.
	void clearData() {cleanupData(ClearRows);}
	//! frees all allocated resources (fields, columns, rows, indexes, etc.)
	void clear() {cleanupData(ClearFieldsRows);}

	bool isEmpty() const {return rowCount() <= 0;}
	bool isValidRow(int row) const;
	bool isValidField(int fld_ix) const {
		return fields().isValidFieldIndex(fld_ix);
	}
public:
	//! return empty row which is not inserted in the table rows
	TableRow isolatedRow();
	virtual int rowCount() const;
	virtual int columnCount() const;
	virtual TableRow& insertRow(int before_row);
	/// vlozi  _row, do tabulky, pokud je to radek z jine tabulky (neziskany metodou singleRow()), tak exception
	/// vetsinou se pouziva pro vlozeni radku ziskaneho funkci singleRow()
	virtual TableRow& insertRow(int before_row, const TableRow &_row);
	virtual TableRow& appendRow() {return insertRow(rowCount());}
	virtual TableRow& appendRow(const TableRow &_row) {return insertRow(rowCount(), _row);}
	virtual bool removeRow(int ri);
	// other related
	void revertRow(int ri);
	virtual void revertRow(TableRow &r);
protected:
	virtual void sort(RowIndexList::iterator begin, RowIndexList::iterator end);
	virtual RowIndexList::const_iterator binaryFind(RowIndexList::const_iterator begin, RowIndexList::const_iterator end, const QVariant &val) const;
public:
	void setSortCollator(const QCollator &coll);
	QCollator sortCollator() const;
	/// sort / search / seek
	//! @param colnames Coma separaded list of fields with optional ASC or DES  and CS or ICS flag.
	//!		CS is CaseSensitive, default is CS.
	//!						For example "id, name DESC CS, salary"
	void sort(const QString &colnames);
	//! For convinience.
	void sort(const SortDef &sorted_field);
	virtual void sort(const SortDefList &sorted_fields);
	//! pokud chci zavolta sort jen pro urcitou objast radku
	virtual void sort(const SortDefList &sorted_fields, int start_row_index, int row_count);
	void resort();
	void setSortDefinition(const SortDefList & sorted_fields) {tablePropertiesRef().setSortDefinition(sorted_fields);}
	//! returns index of \a val or -1. Table must be sorted ascending.
	//! Function seeks in the first sorted field.
	//! If val is QVariantList, function seek in all val values up to sortedFields.count()
	int seek(const QVariant &val) const;
	/// Prochazi tabulku radek po radku, pokus nic nenajde vraci -1
	int find(int field_ix, const QVariant &val) const;
	int find(const QString &field_name, const QVariant &val) const;
public:
	// export / import
	QString toString() const;
	static const QString CVSTableEndTag;
	static QString quoteCSV(const QString &s, const TextExportOptions &opts);
	void exportCSV(QTextStream &ts, const QString col_names = "*", TextExportOptions opts = TextExportOptions()) const;
	void importCSV(QTextStream &ts, TextImportOptions opts = TextImportOptions());
#ifdef TXT_EXPORT_IMPORT
	void importTXT(QTextStream &ts, const QString &file_structure_definition);
#endif
	//void exportExcelXML(QTextStream &ts, const QString col_names = QString(), TextExportOptions opts = TextExportOptions()) const;

	QDomElement toHtmlElement(QDomDocument &owner_doc, const QString & col_names = QString(), TextExportOptions opts = TextExportOptions()) const;
	//QDomElement exportXML(const QString col_names = "*") const;
	//QFXmlTable toXmlTable(QDomDocument &owner_doc, const QString &table_name = QString(), const QString &col_names = QString()) const;
	QVariantMap toJson(const QString &col_names = QString()) const;
	SValue toTreeTable(const QString &col_names = QString(), const QString &table_name = QString()) const;
	/// v pripade neuspechu vraci false
	bool fromTreeTable(const SValue &tree_table, bool throw_exc = true);
	/// ulozi data v tabulce jako QVariantList QVariantListu (kazdy radek je jeden QVariantList)
	QVariantList dataToVariantList() const;
	void dataFromVariantList(const QVariantList &_lst);
public:
	Table();
	Table(const QStringList &col_names);
	Table(const FieldList &col_defs);
	virtual ~Table();
};

//! One row in table, implicitly shared.
class QFCORE_DECL_EXPORT TableRow
{
	//friend class QFSqlQueryTable;
	//friend class QFTableModel;
	//friend class QFSqlQueryModel;
private:
	class SharedDummyHelper {};
	class Data : public QSharedData
	{
		friend class TableRow;
	public:
		QVector<QVariant> values;
		QVector<QVariant> origValues;
		QBitArray dirtyFlags; ///< jsou situace, kdy je treba oznacit field jako dirty a pritom origValue a value jsou stejne
		Table::TableProperties tableProperties;
		struct Flags {
			bool insert:1;
			//bool forcedInsert:1;
		} flags;
	public:
		Data();
		Data(const Table::TableProperties &props);
		~Data();
	};
private:
	QSharedDataPointer<Data> d;
	static const TableRow& sharedNull();
	TableRow(SharedDummyHelper);
public:
	QVector<QVariant>& valuesRef() {return d->values;}
	const QVector<QVariant>& values() const {return d->values;}
	QVariantMap valuesMap() const;
	//QBitArray& nullFlagsRef() {return d->nullFlags;}
public:
	void saveValues();
	void restoreValues();
	void clearEditFlags();
	void clearOrigValues();
	//! Uvede radek do ModeInsert a nastavi vsem fieldum dirty flag
	void prepareForCopy();
protected:
	void setInitialValue(int col, const QVariant &v);
public:
	QVariant origValue(int col) const;
	QVariant origValue(const QString &field_name) const;
	//bool hasNullFlag(int col) const;
	QVariant value(int col) const;
	QVariant value(const QString &field_name) const;
	//! Dirty flag nastavi, jen kdyz je value jina, nez ta, co uz tam byla.
	void setValue(int col, const QVariant &v);
	void setValue(const QString &field_name, const QVariant &v);
	bool isDirty() const;
	bool isDirty(int field_no) const;
	void setDirty(int field_no, bool val = true);

	//void fillDefaultAndAutogeneratedValues();

	//! returns number of fields in the row.
	int fieldCount() const {return fields().count();}
	//! returns number of values in the row, Shoul be the same as \a fieldCount() .
	int count() const {return d->values.count();}
	bool isInsert() const {return d->flags.insert;}
	void setInsert(bool b = true) {d->flags.insert = b;}
	//bool isForcedInsert() const {return d->flags.forcedInsert;}
	/// forcedInsert se pouziva v pripadech, kdy se data zaznamu nachazi ve vice linkovanych tabulkach a behem loadData() se zjisti, ze zaznam v nekterych tabulkach chybi
	/// viz. napr. PPKlientKartaDataFormDocument::loadData()
	//void setForcedInsert(bool b = true) {d->flags.forcedInsert = b;}
	bool isNull() const {return d == sharedNull().d;}
	//void setFields(const FieldList& flds) {d->fields = flds;}
	const Table::TableProperties& tableProperties() const {return d->tableProperties;}
	//TableProperties& tablePropertiesRef() {return d->tableProperties;}
	const Table::FieldList& fields() const {return d->tableProperties.fields();}
	//FieldList& fieldsRef() {return d->tableProperties.fieldsRef();}
	//public:
	//static TableRow nullRow();
	//bool operator<(const TableRow &r) const;

	QString toString(const QString &sep = "\t") const;
public:
	TableRow();
	TableRow(const Table::TableProperties &props);
	//virtual ~TableRow();
};

}}}

#endif
