#include "treetable.h"

#include "../core/log.h"
#include "../core/utils.h"
#include "../core/string.h"
#include "../utils/timescope.h"

using namespace qf::core::utils;
#if 0
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
				int t = QMetaType::type(ts.toLatin1().constData());
				//QVariant::Type t = QVariant::nameToType(qPrintable(ts));
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
#endif

int TreeTableRow::columnIndex(const QString &col_name) const
{
	for (int i = 0; i < m_columns.count(); ++i) {
		QVariantMap cd = m_columns[i].toMap();
		if(Utils::fieldNameEndsWith(cd.value(TreeTable::KEY_NAME).toString(), col_name))
			return i;
	}
	return -1;
}

QVariant TreeTableRow::value(int col_ix) const
{
	if(0 <= col_ix && col_ix < m_columns.count()) {
		QVariant v;
		if(m_row.type() == QVariant::Map) {
			v = m_row.toMap().value(TreeTable::KEY_ROW).toList().value(col_ix);
		}
		else {
			v = m_row.toList().value(col_ix);
		}
		/// pretypuj na typ sloupce
		return retypeVariant(col_ix, v);
	}
	return QVariant();
}

QVariant TreeTableRow::value(const QString &col_or_key_name) const
{
	QVariant v;
	int ix = columnIndex(col_or_key_name);
	if(ix < 0) {
		v = m_row.toMap().value(TreeTable::KEY_KEYVALS).toMap().value(col_or_key_name);
	}
	else {
		v = value(ix);
	}
	return v;
}

QVariant TreeTableRow::retypeVariant(int col_ix, const QVariant &v) const
{
	if(v.isValid()) {
		QVariantMap cd = m_columns.value(col_ix).toMap();
		QString ts = cd.value(TreeTable::KEY_TYPE).toString();
		if(!ts.isEmpty()) {
			int t = QMetaType::type(ts.toUtf8().constData());
			return Utils::retypeVariant(v, t);
		}
	}
	return v;
}

int TreeTableRow::tablesCount() const
{
	if(m_row.type() == QVariant::Map) {
		return m_row.toMap().value(TreeTable::KEY_TABLES).toList().count();
	}
	return 0;
}

TreeTable TreeTableRow::table(int ix) const
{
	if(m_row.type() == QVariant::Map) {
		QVariant t = m_row.toMap().value(TreeTable::KEY_TABLES).toList().value(ix);
		return TreeTable(t);
	}
	return TreeTable();
}

TreeTable TreeTableRow::table(const QString &table_name) const
{
	QVariantList tlst = m_row.toMap().value(TreeTable::KEY_TABLES).toList();
	for (int i = 0; i < tlst.count(); ++i) {
		TreeTable tt{tlst[i]};
		if(tt.name() == table_name)
			return tt;
	}
	return TreeTable();
}

//=================================================
//             TreeTable
//=================================================
const QString TreeTable::KEY_COLUMNS = "fields";
const QString TreeTable::KEY_HEADER = "header";
const QString TreeTable::KEY_FOOTER = "footer";
const QString TreeTable::KEY_HALIGN = "halign";
const QString TreeTable::KEY_VALIGN = "valign";
const QString TreeTable::KEY_WIDTH = "width";
const QString TreeTable::KEY_ROWS = "rows";
const QString TreeTable::KEY_META = "meta";
const QString TreeTable::KEY_NAME = "name";
const QString TreeTable::KEY_TYPE = "type";
const QString TreeTable::KEY_TABLES = "tables";
const QString TreeTable::KEY_ROW = "row";
const QString TreeTable::KEY_KEYVALS = "keyvals";

void TreeTable::setName(const QString &n)
{
	QVariantMap t = m_values.toMap();
	QVariantMap m = t.value(KEY_META).toMap();
	if(n.isEmpty())
		m.remove(KEY_NAME);
	else
		m[KEY_NAME] = n;
	t[KEY_META] = m;
	m_values = t;
}

int TreeTable::insertRow(int ix, const QVariantList &vals)
{
	if(ix < 0)
		ix = 0;
	if(ix >= rowCount())
		ix = rowCount();
	appendRow();
	QVariantList rr = rows();
	for (int i = rr.count() - 1; i > ix; --i) {
		rr[i] = rr[i-1];
	}
	rr[ix] = vals;
	setRows(rr);
	return ix;
}

int TreeTable::appendRow(const QVariantList &vals)
{
	QVariantList rr = rows();
	rr.insert(rr.count(), vals);
	setRows(rr);
	return rowCount() - 1;
}

void TreeTable::removeRow(int ix)
{
	if(ix >= 0 &&  ix < rowCount()) {
		QVariantList rr = rows();
		rr.removeAt(ix);
		setRows(rr);
	}
}

void TreeTable::setColumnWidth(int col_ix, const QVariant &width)
{
	if(col_ix >= 0 && col_ix < columnCount()) {
		QVariantList cc = columns();
		QVariantMap cd = cc[col_ix].toMap();
		if(width.isValid())
			cd[KEY_WIDTH] = width;
		else
			cd.remove(KEY_WIDTH);
		cc[col_ix] = cd;
		setColumns(cc);
	}
}

void TreeTable::setColumnWidth(const QString &col_name, const QVariant &width)
{
	setColumnWidth(columnIndex(col_name), width);
}

QVariant TreeTable::columnWidth(int col_ix) const
{
	return columns().value(col_ix).toMap().value(KEY_WIDTH);
}

QVariant TreeTable::columnWidth(const QString &col_name) const
{
	int ix = columnIndex(col_name);
	return columnWidth(ix);
}

void TreeTable::setColumnHeader(const QString &col_name, const QString &caption)
{
	qfLogFuncFrame();
	int ix = columnIndex(col_name);
	if(ix >= 0) {
		QVariantList cc = columns();
		QVariantMap cd = cc[ix].toMap();
		if(caption.isEmpty())
			cd.remove(KEY_HEADER);
		else
			cd[KEY_HEADER] = caption;
		cc[ix] = cd;
		setColumns(cc);
	}
}

QString TreeTable::columnHeader(int col_ix) const
{
	if(col_ix >= 0 && col_ix < columnCount())
		return columns()[col_ix].toMap().value(KEY_HEADER).toString();
	return QString();
}

QString TreeTable::columnHeader(const QString &col_name) const
{
	QString ret = columnHeader(columnIndex(col_name));
	if(ret.isEmpty())
		ret = col_name;
	return ret;
}

void TreeTable::setColumnFooter(const QString &col_name, const QString &caption)
{
	int ix = columnIndex(col_name);
	if(ix >= 0) {
		QVariantList cc = columns();
		QVariantMap cd = cc[ix].toMap();
		if(caption.isEmpty())
			cd.remove(KEY_FOOTER);
		else
			cd[KEY_FOOTER] = caption;
		cc[ix] = cd;
		setColumns(cc);
	}
}

QString TreeTable::columnFooter(const QString &col_name) const
{
	QString ret = col_name;
	int ix = columnIndex(col_name);
	if(ix >= 0)
		ret = columns()[ix].toMap().value(KEY_FOOTER).toString();
	return ret;
}

void TreeTable::setColumnAlignment(const QString &col_name, Qt::Alignment alignment)
{
	setColumnAlignment(columnIndex(col_name), alignment);
}

void TreeTable::setColumnAlignment(int ix, Qt::Alignment alignment)
{
	qfLogFuncFrame();
	if(ix >= 0) {
		QVariantList cc = columns();
		QVariantMap cd = cc[ix].toMap();

		if(alignment & Qt::AlignLeft) cd[KEY_HALIGN] = QStringLiteral("left");
		else if(alignment & Qt::AlignRight) cd[KEY_HALIGN] = QStringLiteral("right");
		else if(alignment & Qt::AlignCenter || alignment & Qt::AlignHCenter) cd[KEY_HALIGN] = QStringLiteral("center");
		else cd.remove(KEY_HALIGN);

		if(alignment & Qt::AlignTop) cd[KEY_VALIGN] = QStringLiteral("top");
		else if(alignment & Qt::AlignBottom) cd[KEY_VALIGN] = QStringLiteral("bottom");
		else if(alignment & Qt::AlignCenter || alignment & Qt::AlignVCenter) cd[KEY_VALIGN] = QStringLiteral("center");
		else cd.remove(KEY_VALIGN);

		cc[ix] = cd;
		setColumns(cc);
	}
}

int TreeTable::columnType(int col_no) const
{
	QString t = columns().value(col_no).toMap().value(KEY_TYPE).toString();
	return (int)QVariant::nameToType(qPrintable(t));
}

void TreeTable::setColumnType(int col_no, int meta_type_id)
{
	if(col_no < 0 || col_no >= columnCount())
		return;
	QVariantList cc = columns();
	QVariantMap cd = cc[col_no].toMap();
	cd[KEY_TYPE] = QVariant::typeToName(meta_type_id);
	cc[col_no] = cd;
	setColumns(cc);
}

void TreeTable::appendColumn(const QString &name, QVariant::Type type, const QString &caption)
{
	QVariantMap cd;

	cd[KEY_NAME] = name;
	QString type_name = QVariant::typeToName((int)type);
	cd[KEY_TYPE] = type_name;

	if(!caption.isEmpty())
		cd[KEY_HEADER] = caption;
	QVariantList cc = columns();
	cc << cd;
	setColumns(cc);
}

TreeTableRow TreeTable::row(int row_ix) const
{
	return TreeTableRow(columns(), rows().value(row_ix));
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
			int ix = columnIndex(key_name);
			if(ix < 0)
				ret = _key_name;
			else
				ret = columnHeader(key_name);
		}
		else if(key_name.startsWith("HEADER(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(7, -1).trimmed();
			int ix = columnIndex(key_name);
			if(ix < 0)
				ret = _key_name;
			else
				ret = columnHeader(key_name);
		}
		else if(key_name.startsWith("FOOTER(", Qt::CaseInsensitive)) {
			key_name = key_name.slice(7, -1).trimmed();
			int ix = columnIndex(key_name);
			if(ix < 0)
				ret = _key_name;
			else
				ret = columnFooter(key_name);
		}
		else {
			QVariantMap kv = keyvals();
			QMapIterator<QString, QVariant> it(kv);
			while(it.hasNext()) {
				it.next();
				QString key = it.key();
				//qfDebug() << "\t checking key:" << key;
				if(key_ends_with) {
					if(Utils::fieldNameEndsWith(key, key_name)) {
						ret = it.value();
						//qfDebug() << "\t\t ends with" << sv.toString(2);
						break;
					}
				}
				else {
					if(key.compare(key, key_name, Qt::CaseInsensitive) == 0) {
						ret = it.value();
						//qfDebug() << "\t\t equal";
						break;
					}
				}
			}
		}
	}
	qfDebug() << "\treturn:" << ret.toString();
	return ret;
}

void TreeTable::setValue(const QString& key_name, const QVariant& val)
{
	QVariantMap kv = keyvals();
	kv[key_name] = val;
	setKeyvals(kv);
}
#if 0
QVariant TreeTable::value(int row_ix, const QString &col_or_key_name) const
{
	return row(row_ix).value(col_or_key_name);
	/*
	int ix = columnIndex(col_or_key_name);
	if(ix < 0) {
		return rows().value(row_ix).toMap().value(KEY_KEYVALS).toMap().value(col_or_key_name);
	}
	return value(row_ix, ix);
	*/
}

QVariant TreeTable::value(int row_ix, int col_ix) const
{
	return row(row_ix).value(col_ix);
	/*
	QVariant ret;
	QVariant rv = rows().value(row_ix);
	if(rv.type() == QVariant::Map) {
		ret = rv.toMap().value(KEY_ROW).toList().value(col_ix);
	}
	else {
		ret = rv.toList().value(col_ix);
	}
	return ret;
	*/
}
#endif
void TreeTable::setValue(int row_ix, int col_ix, const QVariant &val)
{
	if(row_ix < 0 || row_ix >= rowCount())
		return;
	if(col_ix < 0 || col_ix >= columnCount())
		return;
	QVariantList rr = rows();
	QVariant &rv = rr[row_ix];
	if(rv.type() == QVariant::Map) {
		QVariantMap rm = rv.toMap();
		QVariantList rvals = rm.value(KEY_ROW).toList();
		while (rvals.count() <= col_ix)
			rvals << QVariant();
		rvals[col_ix] = val;
		rm[KEY_ROW] = rvals;
		rv = rm;
	}
	else {
		QVariantList rvals = rv.toList();
		while (rvals.count() <= col_ix)
			rvals << QVariant();
		rvals[col_ix] = val;
		rv = rvals;
	}
	setRows(rr);
}

void TreeTable::setValue(int row_ix, const QString &col_or_key_name, const QVariant &val)
{
	int ix = columnIndex(col_or_key_name);
	if(ix < 0) {
		QVariantList rr = rows();
		QVariant &rv = rr[row_ix];
		QVariantMap rm;
		if(rv.type() != QVariant::Map)
			rm = QVariantMap{ {KEY_ROW, rv} };
		else
			rm = rv.toMap();
		QVariantMap kvals = rm.value(KEY_KEYVALS).toMap();
		if(val.isValid())
			kvals[col_or_key_name] = val;
		else
			kvals.remove(col_or_key_name);
		rm[KEY_KEYVALS] = kvals;
		rv = rm;
		setRows(rr);
	}
	else {
		setValue(row_ix, ix, val);
	}
}

QVariant TreeTable::sum(const QString &col_name) const
{
	//qfDebug() << QF_FUNC_NAME << col_name;
	int ix = columnIndex(col_name);
	if(ix >= 0)
		return sum(ix);
	return QVariant();
}

QVariant TreeTable::sum(int col_index) const
{
	//qfDebug() << QF_FUNC_NAME << "col index:" << col_index;
	if(col_index < 0 || col_index >= columnCount())
		return QVariant();
	QString ts = columns().value(col_index).toMap().value(KEY_TYPE).toString();
	QVariant::Type t = QVariant::nameToType(qPrintable(ts));
	QVariant ret;
	//qfInfo() << "type:" << QVariant::typeToName(t);
	if(t == QVariant::Int) {
		int s = 0;
		for(int i=0; i<rowCount(); i++) {
			s += row(i).value(col_index).toInt();
		}
		ret = s;
	}
	else {
		double s = 0;
		for(int i=0; i<rowCount(); i++) {
			s += row(i).value(col_index).toDouble();
		}
		ret = s;
	}
	return ret;
}

QVariant TreeTable::average(const QString & col_name) const
{
	//qfDebug() << QF_FUNC_NAME << col_name;
	int ix = columnIndex(col_name);
	if(ix >= 0)
		return average(ix);
	return QVariant();
}

QVariant TreeTable::average(int col_index) const
{
	QVariant ret;
	double s = sum(col_index).toDouble();
	int n = rowCount();
	if(n > 0)
		ret = s / n;
	return ret;
}

TreeTable TreeTable::cd(const QString& path) const
{
	qfLogFuncFrame() << "path:" << path;
	qfDebug() << "\t isValid():" << isValid();
	QStringList path_lst = path.split('/', QString::SkipEmptyParts);
	TreeTable ret = *this;
	for(int path_ix=0; path_ix<path_lst.count(); path_ix++) {
		bool ok;
		int row_ix = path_lst.value(path_ix).toInt(&ok);
		path_ix++;
		if(ok) {
			qfDebug() << "\t loading row #" << row_ix;
			QString table_name = path_lst.value(path_ix);
			ret = table(row_ix, table_name);
		}
		else {
			qfError() << QString("cd() - Bad path, path '%1'.").arg(path_lst.join("/"));
			ret = TreeTable();
			break;
		}
	}
	return ret;
}

int TreeTable::tablesCount(int row_ix) const
{
	TreeTableRow ttr(columns(), rows().value(row_ix));
	return ttr.tablesCount();
}

TreeTable TreeTable::table(int row_ix, int table_ix) const
{
	TreeTableRow ttr(columns(), rows().value(row_ix));
	return ttr.table(table_ix);
}

TreeTable TreeTable::table(int row_ix, const QString &table_name) const
{
	TreeTableRow ttr(columns(), rows().value(row_ix));
	return ttr.table(table_name);
}

void TreeTable::appendTable(int row_ix, const TreeTable &t)
{
	if(row_ix < 0 || row_ix >= rowCount()) {
		qfWarning() << "invalid row index:" << row_ix << "of:" << rowCount();
		return;
	}
	QVariantList rr = rows();
	QVariant row = rr.value(row_ix);
	QVariantMap rm;
	if(row.type() == QVariant::List)
		rm[KEY_ROW] = row;
	else
		rm = row.toMap();
	QVariantList tlst = rm.value(KEY_TABLES).toList();
	tlst << t.toVariant();
	rm[KEY_TABLES] = tlst;
	rr[row_ix] = rm;
	setRows(rr);
}

QVariantMap TreeTable::keyvals(int row_ix) const
{
	return rows().value(row_ix).toMap().value(KEY_KEYVALS).toMap();
}

static inline QString line_indent(const QString &ind, int level)
{
	QString s;
	for(int i=0; i<level; i++) s += ind;
	return s;
}
#define IND(level) offset + line_indent(ind, level)

static QString keyvals_to_html(const QVariantMap &keyvals, const QVariantMap &opts)
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
			ret += IND(2) + "<td>" + keyvals.value(s).toString() + "</td>" + eoln;
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
	ret += name();
	ret += IND(0) + "<table border=1>" + eoln;
	QVariantList cc = columns();
	ret += IND(1) + "<tr>" + eoln;
	for(int i=0; i<cc.count(); i++) {
		QVariantMap cd = cc[i].toMap();
		ret += IND(2) + "<th>";
		ret += cd.value(KEY_HEADER).toString();
		ret += "</th>" + eoln;
	}
	ret += IND(1) + "</tr>" + eoln;
	for(int i=0; i<rowCount(); i++) {
		TreeTableRow rr = row(i);
		ret += IND(1) + "<tr>" + eoln;
		for(int j=0; j<cc.count(); j++) {
			ret += IND(2) + "<td>";
			ret += rr.value(j).toString();
			ret += "</td>" + eoln;
		}
		ret += IND(1) + "</tr>" + eoln;
		opts2["lineOffset"] = IND(3);
		for(int j=0; j<tablesCount(i); j++) {
			TreeTable t = table(i, j);
			ret += IND(1) + "<tr>" + eoln;
			ret += IND(2) + "<td colspan=" QF_IARG(cc.count()) ">" + eoln;
			ret += t.toHtml(opts2);
			ret += IND(2) + "</td>" + eoln;
			ret += IND(1) + "</tr>" + eoln;
		}
		QVariantMap kv = keyvals(i);
		if(kv.count()) {
			ret += IND(1) + "<tr>" + eoln;
			ret += IND(2) + "<td colspan=" QF_IARG(cc.count()) ">" + eoln;
			opts2["lineOffset"] = IND(3);
			ret += keyvals_to_html(kv, opts2);
			ret += IND(2) + "</td>" + eoln;
			ret += IND(1) + "</tr>" + eoln;
		}
	}
	ret += IND(0) + "</table>" + eoln;
	/// keyvals
	auto kvs = keyvals();
	if(kvs.count()) {
		opts2["lineOffset"] = IND(0);
		ret += keyvals_to_html(kvs, opts2);
	}
	return ret;
}

QByteArray TreeTable::toJson(QJsonDocument::JsonFormat format) const
{
	QJsonDocument doc = QJsonDocument::fromVariant(toVariant());
	QByteArray ba = doc.toJson(format);
	return ba;
}

QString TreeTable::toString(QJsonDocument::JsonFormat format) const
{
	auto ba = toJson(format);
	return QString::fromUtf8(ba);
}
/*
QVariant TreeTable::retypeVariant(int col_ix, const QVariant &v) const
{
	if(v.isValid()) {
		QVariantMap cd = columns().value(col_ix).toMap();
		QString ts = cd.value(TreeTable::KEY_TYPE).toString();
		if(!ts.isEmpty()) {
			int t = QMetaType::type(ts.toUtf8().constData());
			return Utils::retypeVariant(v, t);
		}
	}
	return v;
}
*/
void TreeTable::setKeyvals(const QVariantMap &keyvals)
{
	QVariantMap t = valueMap();
	t[KEY_KEYVALS] = keyvals;
	m_values = t;
}

void TreeTable::setColumns(const QVariantList &cols)
{
	QVariantMap t = valueMap();
	t[KEY_COLUMNS] = cols;
	m_values = t;
}

int TreeTable::columnIndex(const QString &col_name) const
{
	QVariantList cc = columns();
	for (int i = 0; i < cc.count(); ++i) {
		QVariantMap cd = cc[i].toMap();
		if(Utils::fieldNameEndsWith(cd.value(KEY_NAME).toString(), col_name))
			return i;
	}
	return -1;
}

void TreeTable::setRows(const QVariantList &rows)
{
	QVariantMap t = valueMap();
	t[KEY_ROWS] = rows;
	m_values = t;
}

