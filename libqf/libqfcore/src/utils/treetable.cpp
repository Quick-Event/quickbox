
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#include "treetable.h"

#include "../core/log.h"
#include "../core/utils.h"
#include "../core/string.h"
#include "../utils/timescope.h"

using namespace qf::core::utils;

//=================================================
//             TreeTableColumn
//=================================================
QString TreeTableColumn::name() const
{
	return property(TreeTable::KEY_NAME).toString();
}

void TreeTableColumn::setName(const QString& n)
{
	setProperty(TreeTable::KEY_NAME, n);
}

QString TreeTableColumn::typeName() const
{
	return property(TreeTable::KEY_TYPE).toString();
}

void TreeTableColumn::setTypeName(const QString& tn)
{
	if(QVariant::nameToType(qPrintable(tn)) == QVariant::Invalid) {
		qfError() << "Invalid type name" << tn;
	}
	else {
		setProperty(TreeTable::KEY_TYPE, tn);
	}
}

QVariant::Type TreeTableColumn::type() const
{
	return QVariant::nameToType(qPrintable(typeName()));
}

void TreeTableColumn::setType(QVariant::Type t)
{
	QString type_name = QVariant::typeToName(t);
	//qfInfo() << "type:" << t << "type name:" << type_name;
	setProperty(TreeTable::KEY_TYPE, type_name);
}

QString TreeTableColumn::width() const
{
	QString ret = property("width").toString();
	return ret;
}

void TreeTableColumn::setWidth(const QString& w)
{
	QVariant v;
	if(!w.trimmed().isEmpty()) v = w.trimmed();
	setProperty("width", v);
}

QString TreeTableColumn::header() const
{
	QString ret = property("header").toString();
	if(ret.isEmpty())
		Utils::parseFieldName(name(), &ret);
	return ret;
}

QString TreeTableColumn::footer() const
{
	QString ret = property("footer").toString();
	return ret;
}

QString TreeTableColumn::hAlignment() const
{
	QString ret = property("halign").toString();
	if(ret.isEmpty()) {
		/// pokud neni hodnota predepsana, vem ji z dat
		QVariant::Type type;
		QVariant t = property(TreeTable::KEY_TYPE);
		if(t.type() == QVariant::String) type = QVariant::nameToType(qPrintable(t.toString()));
		else type = (QVariant::Type)t.toInt();
		switch(type) {
			case QVariant::Int:
			case QVariant::UInt:
			case QVariant::LongLong:
			case QVariant::ULongLong:
			case QVariant::Double:
				ret = "right";
				break;
			default:
				break;
		}
	}
	return ret;
}

//=================================================
//             TreeTableColumns
//=================================================
TreeTableColumns::TreeTableColumns(const QVariant &v)
	: SValue(v)
{
}

int TreeTableColumns::indexOf(const QString &col_name) const
{
	for(int i=0; i<count(); i++) {
		TreeTableColumn col = column(i);
		if(Utils::fieldNameEndsWith(col.name(), col_name)) {
			return i;
		}
	}
	return -1;
}

//=================================================
//             TreeTableRow
//=================================================
TreeTableRow::TreeTableRow(const TreeTableColumns &columns, const SValue &row_data)
{
	f_columns = columns;
	f_row = row_data;
}

TreeTableRow::~TreeTableRow()
{
}

QVariant TreeTableRow::value_helper(int col, bool &found) const
{
	QVariant ret;
	found = false;
	if(0 <= col && col < columns().count()) {
		found = true;
		QVariant v;
		if(f_row.isList()) v = f_row.property(col);
		else if(f_row.isMap()) {
			SValue sv = f_row.property(TreeTable::KEY_ROW);
			v = sv.property(col);
		}
		ret = v;
		if(ret.userType() == qMetaTypeId<SValue>()) {
			//ret = v.value<SValue>().value();
			//qfWarning() << "property column:" << col << "is SValue.\n" << ret.value<SValue>().toString(2);
		}
		else {
			/// pretypuj na typ sloupce
			if(v.isValid()) {
				TreeTableColumn cd = columns()[col];
				QString ts = cd.property(TreeTable::KEY_TYPE).toString();
				QVariant::Type t = QVariant::nameToType(qPrintable(ts));
				ret = Utils::retypeVariant(v, t);
			}
		}
	}
	return ret;
}

QVariant TreeTableRow::value(int ix) const
{
	//qfLogFuncFrame() << col_or_key_name;
	bool found;
	QVariant ret = value_helper(ix, found);
	if(!found) {
		qfError() << QString("index %1 not found, column count: %2").arg(ix).arg(columns().count());
	}
	//qfDebug() << "\t return:" << ret.toString() << "type:" << ret.typeName();
	return ret;
}

QVariant TreeTableRow::value(int ix, const QVariant &default_val) const
{
	bool found;
	QVariant ret = value_helper(ix, found);
	if(!found) {
		ret = default_val;
	}
	return ret;
}

QVariant TreeTableRow::value_helper(const QString &col_or_key_name, bool &found) const
{
	qfLogFuncFrame() << col_or_key_name;
	QVariant ret;
	found = true;
	int ix = columns().indexOf(col_or_key_name);
	if(ix < 0) {
		if(f_row.isMap()) {
			/// try keyvals
			SValue sv = f_row.property(TreeTable::KEY_KEYVALS);
			//qfInfo() << col_or_key_name << "->" << sv.toString(2);
			found = sv.hasProperty(col_or_key_name, &ret);
		}
		else
			found = false;
	}
	else ret = value_helper(ix, found);
	qfDebug() << "\t return:" << ret.toString() << "type:" << ret.typeName();
	return ret;
}

QVariant TreeTableRow::value(const QString &col_or_key_name) const
{
	qfLogFuncFrame() << col_or_key_name;
	bool found;
	QVariant ret = value_helper(col_or_key_name, found);
	if(!found) {
		SValue sv = f_row.property(TreeTable::KEY_KEYVALS);
		QStringList cols;
		{
			auto cc = columns();
			for(int i=0; i<cc.count(); i++) {
				TreeTableColumn c = cc.column(i);
				cols << c.name();
			}
		}
		QStringList keys = sv.keys();
		qfError() << QString("key '%1' not found in columns or keyvals\n\tcolumns: %2\n\tkeys: %3").arg(col_or_key_name).arg(cols.join(", ")).arg(keys.join(", "));
	}
	qfDebug() << "\t return:" << ret.toString() << "type:" << ret.typeName();
	return ret;
}

QVariant TreeTableRow::value(const QString &col_or_key_name, const QVariant &default_val) const
{
	qfLogFuncFrame() << col_or_key_name;
	bool found;
	QVariant ret = value_helper(col_or_key_name, found);
	if(!found) {
		ret = default_val;
	}
	qfDebug() << "\t return:" << ret.toString() << "type:" << ret.typeName();
	return ret;
}

void TreeTableRow::setValue(int col, const QVariant &val)
{
	if(f_row.isList()) {
		f_row.setProperty(col, val);
	}
	else if(f_row.isMap()) {
		SValue sv = f_row[TreeTable::KEY_ROW];
		sv.setProperty(col, val);
	}
	else {
		/// at je tam co chce, udelej z toho radek
		//if(f_row.isValid()) f_row.setValue(QVariant()); /// vyhni se warningu
		f_row.setProperty(col, val);
	}
}

void TreeTableRow::setValue(const QString &col_or_key_name, const QVariant &val)
{
	//if(isNull()) return;
	int ix = columns().indexOf(col_or_key_name);
	if(ix < 0) {
		convertListToMap();
		SValue sv = f_row[TreeTable::KEY_KEYVALS];
		sv.setProperty(col_or_key_name, val);
	}
	else setValue(ix, val);
}

SValue TreeTableRow::keyvals() const
{
	SValue ret;
	if(f_row.isMap()) {
		//qfInfo() << "je to mapa2";
		ret = f_row.property(TreeTable::KEY_KEYVALS);
	}
	return ret;
}

SValue TreeTableRow::keyvalsRef()
{
	SValue ret;
	convertListToMap();
	if(f_row.isMap()) {
		//qfInfo() << "je to mapa";
		ret = f_row[TreeTable::KEY_KEYVALS];
	}
	return ret;
}

int TreeTableRow::tablesCount() const
{
	int ret = 0;
	if(f_row.isMap()) {
		SValue sv = f_row.property(TreeTable::KEY_TABLES);
		ret = sv.count();
	}
	return ret;
}

SValue TreeTableRow::table(int ix) const
{
	SValue ret;
	if(f_row.isMap()) {
		SValue sv = f_row.property(TreeTable::KEY_TABLES);
		ret = sv.property(ix);
	}
	return ret;
}

SValue TreeTableRow::table(const QString &name) const
{
	for(int i=0; i<tablesCount(); i++) {
		TreeTable t = table(i);
		if(name.isEmpty()) return t;
		if(t.name() == name) return t;
	}
	return SValue();
}

void TreeTableRow::convertListToMap()
{
	if(f_row.isList()) {
		/// predelat list na mapu
		QVariantMap m;
		m[TreeTable::KEY_ROW] = f_row.value();
		f_row.setValue(m);
	}
}

void TreeTableRow::appendTable(const SValue &t)
{
	convertListToMap();
	f_row[TreeTable::KEY_TABLES].setProperty(tablesCount(), t);//t.value();
}

//=================================================
//             TreeTable
//=================================================
const QString TreeTable::KEY_COLUMNS = "fields";
const QString TreeTable::KEY_ROWS = "rows";
const QString TreeTable::KEY_META = "meta";
const QString TreeTable::KEY_NAME = "name";
const QString TreeTable::KEY_TYPE = "type";
const QString TreeTable::KEY_TABLES = "tables";
const QString TreeTable::KEY_ROW = "row";
const QString TreeTable::KEY_KEYVALS = "keyvals";

TreeTable::TreeTable()
: SValue()
{
}

TreeTable::~TreeTable()
{
}

int TreeTable::rowCount() const
{
	QF_TIME_SCOPE("RTreeTable::rowCount");
	SValue rs = rows();
	int ret = rs.count();
	return ret;
}

TreeTableRow TreeTable::appendRow()
{
	SValue row_data = (*this)[KEY_ROWS][rowCount()];
	return TreeTableRow(columns(), row_data);
}

void TreeTable::removeRow(int ix)
{
	if(ix >= 0 &&  ix < rowCount()) {
		SValue sv = rows();
		QVariantList vl = sv.value().toList();
		vl.removeAt(ix);
		sv.setValue(vl);
	}
}

void TreeTable::setColumnHeader(const QString &col_name, const QString &caption)
{
	qfLogFuncFrame();
	int ix = columns().indexOf(col_name);
	if(ix >= 0) {
		SValue sv = columns()[ix];
		sv.setProperty("header", caption);
	}
}

QString TreeTable::columnHeader(const QString &col_name) const
{
	QString ret = col_name;
	int ix = columns().indexOf(col_name);
	if(ix >= 0) ret = columns().column(ix).header();
	return ret;
}

void TreeTable::setColumnFooter(const QString &col_name, const QString &caption)
{
	qfLogFuncFrame();
	int ix = columns().indexOf(col_name);
	if(ix >= 0) {
		SValue sv = columns()[ix];
		sv.setProperty("footer", caption);
	}
}

QString TreeTable::columnFooter(const QString &col_name) const
{
	QString ret;
	int ix = columns().indexOf(col_name);
	if(ix >= 0) ret = columns().column(ix).footer();
	return ret;
}
/*
QString TreeTable::columnHAlignment(const QString &col_name) const
{
	QString ret;
	int ix = columns().indexOf(col_name);
	if(ix < 0)
		return ret;
	TreeTableColumn col = columns().column(ix);
	ret = col.hAlignment();
	return ret;
}
*/
/*
QString TreeTable::columnWidth(int col_no) const
{
	TreeTableColumn col = columns().column(col_no);
	ret = col.width();
	return ret;
}
*/
void TreeTable::setColumnAlignment(const QString &col_name, Qt::Alignment alignment)
{
	qfLogFuncFrame();
	int ix = columns().indexOf(col_name);
	setColumnAlignment(ix, alignment);
}

void TreeTable::setColumnAlignment(int ix, Qt::Alignment alignment)
{
	qfLogFuncFrame();
	if(ix >= 0) {
		SValue col = columns().property(ix);

		if(alignment & Qt::AlignLeft) col.setProperty("halign", "left");
		else if(alignment & Qt::AlignRight) col.setProperty("halign", "right");
		else if(alignment & Qt::AlignCenter || alignment & Qt::AlignHCenter) col.setProperty("halign", "center");
		else col.setProperty("halign", QVariant());

		if(alignment & Qt::AlignTop) col.setProperty("valign", "top");
		else if(alignment & Qt::AlignBottom) col.setProperty("valign", "bottom");
		else if(alignment & Qt::AlignCenter || alignment & Qt::AlignVCenter) col.setProperty("valign", "center");
		else col.setProperty("valign", QVariant());
	}
}

TreeTableColumn TreeTable::appendColumn(const QString &name, QVariant::Type type, const QString &caption)
{
	TreeTableColumn col = (*this)[KEY_COLUMNS][columnCount()];
	col.setName(name);
	col.setType(type);
	QString s = caption;
	if(!s.isEmpty()) col.setProperty("header", s);
	return col;
}

QVariant TreeTable::value(const QString &_key_name, const QVariant &default_val, bool key_ends_with) const
{
	qfLogFuncFrame() << _key_name;
	String key_name = _key_name.trimmed();
	QVariant ret = default_val;
	QString path;
	{
		int ix = key_name.lastIndexOf('/');
		if(ix >= 0) {
			path = key_name.mid(0, ix+1);
			key_name = key_name.mid(ix+1);
		}
	}
	//qfDebug() << "\t table is null:" << t.isNull();
	//qfDebug() << "\t path:" << path;
	//qfDebug() << "\t data_src:" << ds;
	if(!path.isEmpty()) {
		TreeTable t = cd(path);
		ret = t.value(key_name);
	}
	else {
		if(key_name.startsWith("SUM(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(4, -1).trimmed();
			ret = sum(key_name);
		}
		else if(key_name.startsWith("AVG(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(4, -1).trimmed();
			ret = average(key_name);
		}
		else if(key_name.startsWith("CNT(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(4, -1).trimmed();
			ret = rowCount();
		}
		else if(key_name.startsWith("CAPTION(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(8, -1).trimmed();
			int ix = columns().indexOf(key_name);
			if(ix < 0) ret = _key_name;
			else ret = columnHeader(key_name);
		}
		else if(key_name.startsWith("HEADER(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(7, -1).trimmed();
			int ix = columns().indexOf(key_name);
			if(ix < 0) ret = _key_name;
			else ret = columnHeader(key_name);
		}
		else if(key_name.startsWith("FOOTER(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(7, -1).trimmed();
			int ix = columns().indexOf(key_name);
			if(ix < 0) ret = _key_name;
			else ret = columnFooter(key_name);
		}
		else {
			SValue sv = property(KEY_KEYVALS);
			foreach(QString key, sv.keys()) {
				//qfDebug() << "\t checking key:" << key;
				if(key_ends_with) {
					if(Utils::fieldNameEndsWith(key, key_name)) {
						ret = sv.property(key);
						//qfDebug() << "\t\t ends with" << sv.toString(2);
						break;
					}
				}
				else {
					if(key.compare(key, key_name, Qt::CaseInsensitive) == 0) {
						ret = sv.property(key);
						//qfDebug() << "\t\t equal";
						break;
					}
				}
			}
		}
	}
	if(ret.userType() == qMetaTypeId<SValue>()) {
		SValue sv = ret.value<SValue>();
		ret = sv.toVariant();
	}
	qfDebug() << "\treturn:" << ret.toString();
	return ret;
}

void TreeTable::setValue(const QString& key_name, const QVariant& val)
{
	keyvals().setProperty(key_name, val);
}

SValue TreeTable::keyvals()
{
	SValue ret = (*this)[KEY_KEYVALS];
	return ret;
}

const SValue TreeTable::keyvals() const
{
	SValue ret;
	if(isMap()) {
		ret = property(KEY_KEYVALS);
	}
	return ret;
}

QVariant TreeTable::sum(const QString &col_name) const
{
	//qfDebug() << QF_FUNC_NAME << col_name;
	int ix = columns().indexOf(col_name);
	if(ix >= 0) return sum(ix);
	return QVariant();
}

QVariant TreeTable::sum(int col_index) const
{
	//qfDebug() << QF_FUNC_NAME << "col index:" << col_index;
	if(col_index < 0 || col_index >= columnCount()) return QVariant();
	QString ts = columns()[col_index].property(KEY_TYPE).toString();
	QVariant::Type t = QVariant::nameToType(qPrintable(ts));
	QVariant ret;
	//qfInfo() << "type:" << QVariant::typeToName(t);
	if(t == QVariant::Int) {
		int s = 0;
		for(int i=0; i<rowCount(); i++) {
			TreeTableRow r = row(i);
			s += r.value(col_index).toInt();
		}
		ret = s;
	}
	else {
		double s = 0;
		for(int i=0; i<rowCount(); i++) {
			TreeTableRow r = row(i);
			s += r.value(col_index).toDouble();
		}
		ret = s;
	}
	return ret;
}

QVariant TreeTable::average(const QString & col_name) const
{
	//qfDebug() << QF_FUNC_NAME << col_name;
	int ix = columns().indexOf(col_name);
	if(ix >= 0) return average(ix);
	return QVariant();
}

QVariant TreeTable::average(int col_index) const
{
	QVariant ret;
	double s = sum(col_index).toDouble();
	int n = rowCount();
	if(n > 0) ret = s / n;
	return ret;
}

QString TreeTable::tableName() const
{
	SValue sv = property(KEY_META);
	return sv.property("name").toString();
}

void TreeTable::setTableName(const QString& name)
{
	(*this)[KEY_META].setProperty("name", name);
}

TreeTable TreeTable::cd(const QString& path) const
{
	qfLogFuncFrame() << "path:" << path;
	qfDebug() << "\t isNull():" << isNull();
	QStringList path_lst = path.split('/', QString::SkipEmptyParts);
	TreeTable ret = *this;
	for(int path_ix=0; path_ix<path_lst.count(); path_ix++) {
		int n = path_lst.value(path_ix).toInt();
		qfDebug() << "\t loading row #" << n;
		TreeTableRow r = ret.row(n);
		path_ix++;
		if(path_ix < path_lst.count()) {
			QString table_name = path_lst.value(path_ix);
			qfDebug() << "\t loading table:" << table_name;
			ret = r.table(table_name);
		}
		else {
			qfError() << QString("cd() - Bad path format, path '%1' is too short.").arg(path_lst.join("/"));
			ret = TreeTable();
			break;
		}
	}
	return ret;
}

static inline QString line_indent(const QString &ind, int level)
{
	QString s;
	for(int i=0; i<level; i++) s += ind;
	return s;
}
#define IND(level) offset + line_indent(ind, level)

static QString keyvals_to_html(const SValue &keyvals, const QVariantMap &opts)
{
	QString offset = opts.value("lineOffset").toString();
	QString eoln = opts.value("lineSeparator", "\n").toString();
	QString ind = opts.value("lineIndent", "  ").toString();
	QString ret;
	QStringList sl = keyvals.keys();
	if(!sl.isEmpty()) {
		ret += IND(0) + "<table border=1>" + eoln;
		foreach(QString s, sl) {
			ret += IND(1) + "<tr>" + eoln;
			ret += IND(2) + "<th>" + s + "</th>" + eoln;
			ret += IND(2) + "<td>" + keyvals.property(s).toString() + "</td>" + eoln;
			ret += IND(1) + "</tr>" + eoln;
		}
		ret += IND(0) + "</table>" + eoln;
	}
	return ret;
}

QString TreeTable::toHtml(const QVariantMap &opts) const
{
	QVariantMap opts2 = opts;
	QString offset = opts.value("lineOffset").toString();
	QString eoln = opts.value("lineSeparator", "\n").toString();
	QString ind = opts.value("lineIndent", "  ").toString();
	QString ret;
	ret += tableName();
	ret += IND(0) + "<table border=1>" + eoln;
	TreeTableColumns cols = columns();
	ret += IND(1) + "<tr>" + eoln;
	for(int i=0; i<cols.count(); i++) {
		TreeTableColumn cd = cols.column(i);
		ret += IND(2) + "<th>";
		ret += cd.header();
		ret += "</th>" + eoln;
	}
	ret += IND(1) + "</tr>" + eoln;
	for(int i=0; i<rowCount(); i++) {
		TreeTableRow r = row(i);
		ret += IND(1) + "<tr>" + eoln;
		for(int i=0; i<cols.count(); i++) {
			ret += IND(2) + "<td>";
			ret += r.value(i).toString();
			ret += "</td>" + eoln;
		}
		ret += IND(1) + "</tr>" + eoln;
		opts2["lineOffset"] = IND(3);
		for(int j=0; j<r.tablesCount(); j++) {
			TreeTable t = r.table(j);
			ret += IND(1) + "<tr>" + eoln;
			ret += IND(2) + "<td colspan=" QF_IARG(cols.count()) ">" + eoln;
			ret += t.toHtml(opts2);
			ret += IND(2) + "</td>" + eoln;
			ret += IND(1) + "</tr>" + eoln;
		}
		SValue keyvals = r.keyvals();
		if(keyvals.count()) {
			ret += IND(1) + "<tr>" + eoln;
			ret += IND(2) + "<td colspan=" QF_IARG(cols.count()) ">" + eoln;
			opts2["lineOffset"] = IND(3);
			ret += keyvals_to_html(keyvals, opts2);
			ret += IND(2) + "</td>" + eoln;
			ret += IND(1) + "</tr>" + eoln;
		}
	}
	ret += IND(0) + "</table>" + eoln;
	/// keyvals
	SValue kvs = keyvals();
	if(kvs.count()) {
		opts2["lineOffset"] = IND(0);
		ret += keyvals_to_html(kvs, opts2);
	}
	return ret;
}

