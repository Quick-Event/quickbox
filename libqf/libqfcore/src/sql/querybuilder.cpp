#include "querybuilder.h"

#include "../core/log.h"
#include "../core/string.h"

#include <QStringBuilder>

using namespace qf::core::sql;

//==================================================================
//            QueryBuilder
//==================================================================
QueryBuilder::QueryBuilder()
{
}

QueryBuilder::~QueryBuilder()
{
}

QString QueryBuilder::buildQuery() const
{
	qfLogFuncFrame();
	QString s, qs;
	s = buildSelect();
	if(!s.isEmpty())
		qs = "SELECT " + s;
	s = buildFrom();
	if(!s.isEmpty())
		qs += " FROM " + s;
	s = buildWhere();
	if(!s.isEmpty())
		qs += " WHERE " + s;
	s = buildGroupBy();
	if(!s.isEmpty())
		qs += " GROUP BY " + s;
	s = buildHaving();
	if(!s.isEmpty())
		qs += " HAVING " + s;
	s = buildOrderBy();
	if(!s.isEmpty())
		qs += " ORDER BY " + s;
	s = buildRest();
	if(!s.isEmpty())
		qs += " " + s;
	if(m_queryMap.contains(AsKey))
		qs = "(" + qs + ") AS " + m_queryMap[AsKey];
	qfDebug() << "\t" << qs;
	return qs;
}

QString QueryBuilder::build(QueryMapKey key) const
{
	QString s = m_queryMap.value(key);
	return s;
}

QString QueryBuilder::buildSelect() const
{
	static const QueryMapKey key = SelectKey;
	QString s = m_queryMap.value(SelectFlagsKey);
	if(!s.isEmpty())
		s += " ";
	s += build(key);
	return s;
}

QString QueryBuilder::buildRest() const
{
	QString s, ret;
	s = build(LimitKey);
	if(!s.isEmpty()) ret += " LIMIT " + s;
	s = build(OffsetKey);
	if(!s.isEmpty()) ret += " OFFSET " + s;
	return ret;
}

QueryBuilder& QueryBuilder::select(const QString &fields, const QString &flags)
{
	static const QueryMapKey key = SelectKey;
	if(!flags.isEmpty()) m_queryMap[SelectFlagsKey] = flags;
	QString s = m_queryMap[key];
	if(!s.isEmpty()) s += ", ";
	s += fields;
	m_queryMap[key] = s;
	return *this;
}

QueryBuilder & QueryBuilder::select2(const QString & table_name, const QString & fields, const QString & flags)
{
	String s = fields;
	QStringList sl = s.splitAndTrim(',', '\'');
	QStringList sl2;
	foreach(s, sl) sl2 << table_name + "." + s;
	s = sl2.join(",");
	return select(s, flags);
}

QueryBuilder& QueryBuilder::from(const QString &_table_name)
{
	static const QueryMapKey key = FromKey;
	qfLogFuncFrame();
	QString table_name = _table_name.trimmed();
	//String s = m_queryMap[key];
	int ix = table_name.lastIndexOf(' ');
	qfDebug() << "\t table_name:" << table_name << "ix:" << ix;
	m_queryMap[key] = table_name;
	return *this;
}

QueryBuilder& QueryBuilder::from(const QueryBuilder &table)
{
	QString table_query = table.toString();
	return from(table_query);
}

namespace
{
typedef QPair< QString, QString > TableKey;
TableKey splitColumnName(const QString &column_name)
{
	TableKey ret;
	int ix = column_name.lastIndexOf('.');
	if(ix > 0) {
		ret.first = column_name.mid(0, ix).trimmed();
		ret.second = column_name.mid(ix+1).trimmed();
	}
	return ret;
}

}

QueryBuilder& QueryBuilder::join2(const QString &t1_key, const QString &_t2_name, const QString &t2_key, const QString  &join_kind)
{
	qfLogFuncFrame();
	static const QueryMapKey key = FromKey;
	TableKey tk1 = splitColumnName(t1_key);
	if(tk1.first.isEmpty()) {
		qfError() << "Table key" + t1_key + "should have form of tablename.fieldname";
		return *this;
	}
	QString t2_name = _t2_name.trimmed();
	QString t2_alias = t2_name;
	int ix = t2_name.lastIndexOf(' ');
	if(ix > 0) {
		t2_alias = t2_name.mid(ix+1).trimmed();
	}
	QString fs = join_kind + " " + t2_name + " ON " + tk1.first + "." + tk1.second + "=" + t2_alias + "." + t2_key;
	QString s = m_queryMap[key];
	if(!s.isEmpty()) s += " ";
	s += fs;
	m_queryMap[key] = s;
	return *this;
}

QueryBuilder& QueryBuilder::join(const QString &join)
{
	qfLogFuncFrame();
	static const QueryMapKey key = FromKey;
	QString s = m_queryMap[key];
	if(!s.isEmpty()) s += " ";
	s += join;
	m_queryMap[key] = s;
	return *this;
}

QueryBuilder& QueryBuilder::join(const QString &t1_key, const QString &t2_key, const QString  &join_kind)
{
	qfLogFuncFrame();
	TableKey tk2 = splitColumnName(t2_key);
	if(tk2.first.isEmpty()) {
		qfError() << "Table key" + t2_key + "should have form of tablename.fieldname";
		return *this;
	}
	return join2(t1_key, tk2.first, tk2.second, join_kind);
}

QueryBuilder& QueryBuilder::joinRestricted(const QString& t1_key, const QString& t2_key, const QString& where_restriction, const QString  &join_kind)
{
	qfLogFuncFrame();
	join(t1_key, t2_key, join_kind);
	if(!where_restriction.isEmpty()) {
		QString s = m_queryMap[FromKey];
		s += " AND ("%where_restriction%')';
		m_queryMap[FromKey] = s;
	}
	return *this;
}

QueryBuilder& QueryBuilder::joinRestrictedAliased(const QString& t1_key, const QString& t2_name, const QString& t2_key, const QString& where_restriction, const QString  &join_kind)
{
	qfLogFuncFrame();
	join2(t1_key, t2_name, t2_key, join_kind);
	if(!where_restriction.isEmpty()) {
		QString s = m_queryMap[FromKey];
		s += " AND (" % where_restriction % ')';
		m_queryMap[FromKey] = s;
	}
	return *this;
}

QueryBuilder& QueryBuilder::join(const QString &t1_key, const QueryBuilder &t2, const QString &t2_key, const QString  &join_kind)
{
	QString table_query = t2.toString();
	return join2(t1_key, table_query, t2_key, join_kind);
}

QueryBuilder& QueryBuilder::where(const QString &cond, const QString &oper)
{
	static const QueryMapKey key = WhereKey;
	if(!cond.isEmpty()) {
		String s = m_queryMap[key];
		if(!s.isEmpty()) s += " " + oper + " ";
		m_queryMap[key] = s + "(" + cond + ")";
	}
	return *this;
}

QueryBuilder& QueryBuilder::groupBy(const QString &expr)
{
	static const QueryMapKey key = GroupByKey;
	String s = m_queryMap[key];
	if(!s.isEmpty()) s += " ";
	m_queryMap[key] = s + expr;
	return *this;
}

QueryBuilder& QueryBuilder::having(const QString &cond, const QString &oper)
{
	static const QueryMapKey key = HavingKey;
	if(!cond.isEmpty()) {
		String s = m_queryMap[key];
		if(!s.isEmpty()) s += " " + oper + " ";
		m_queryMap[key] = s + "(" + cond + ")";
	}
	return *this;
}

QueryBuilder& QueryBuilder::orderBy(const QString &field_and_order_flag)
{
	static const QueryMapKey key = OrderByKey;
	String s = m_queryMap[key];
	if(!s.isEmpty()) s += ", ";
	m_queryMap[key] = s + field_and_order_flag;
	return *this;
}

QueryBuilder& QueryBuilder::limit(int n)
{
	m_queryMap[LimitKey] = QString::number(n);
	return *this;
}

QueryBuilder& QueryBuilder::offset(int n)
{
	m_queryMap[OffsetKey] = QString::number(n);
	return *this;
}

QueryBuilder& QueryBuilder::as(const QString &alias_name)
{
	m_queryMap[AsKey] = alias_name;
	return *this;
}

QString QueryBuilder::take(QueryMapKey key)
{
	QString ret = m_queryMap.take(key);
	//qfInfo() << __LINE__ << ret;
	//foreach(QueryMapKey i, m_queryMap.keys()) qfInfo() << "\t" << i << ":" << m_queryMap[i];
	return ret;
}

void QueryBuilder::clear()
{
	m_queryMap.clear();
}
/*
static int keyword_pos(const QString &str, const QString &keyword)
{
    // tady je trochu problem, ze se musi brat v uvahu dvoje uvozovky ' a "
    // navic where muze byt ve vnorenym selectu
	String fs = str, kw = keyword.toLower();
	int pos;
	int parcnt = 0;
	bool in_apos = false;
	bool in_quotes = false;
	for(pos=0; pos<fs.len(); pos++) {
		QChar c = fs[pos].toLower();
		if(c == '\'') {in_apos = !in_apos; continue;}
		if(c == '"') {in_quotes = !in_quotes; continue;}
		if(!in_apos && !in_quotes) {
			if(c == '(') {parcnt++; continue;}
			else if(c == ')') {parcnt--; continue;}
		}
		if(parcnt == 0 && c == kw[0]) {
			if(fs.slice(pos, pos + kw.len()).toLower() == kw) break;
		}
	}
	if(pos >= fs.len()) pos = -1;
	return pos;
}
*/
#if 0
struct KeyPair
{
	QString sqlName;
	QueryBuilder::QueryMapKey key;
	KeyPair(const QString &sql_name, QueryBuilder::QueryMapKey _key) : sqlName(sql_name), key(_key) {}
};

bool QueryBuilder::parse(const QString &query_string, bool throw_exc) throw(QFSqlException)
{
	qfLogFuncFrame();
	qfDebug() << "\toriginal:" << query_string;
	d->recentlyParsedQuery = query_string;
	String s = query_string.trimmed();
	m_queryMap.clear();
	if(!s) return true;
	static QList<KeyPair> keywords;
	if(keywords.isEmpty()) {
		//keywords << KeyPair("SELECT", SelectKey);
		keywords << KeyPair("FROM", FromKey);
		keywords << KeyPair("WHERE", WhereKey);
		keywords << KeyPair("GROUP BY", GroupByKey);
		keywords << KeyPair("HAVING", HavingKey);
		keywords << KeyPair("ORDER BY", OrderByKey);
		keywords << KeyPair("LIMIT", LimitKey);
		keywords << KeyPair("OFFSET", OffsetKey);
	}

	bool can_parse = true;
	if(keyword_pos(s, "SELECT") != 0) {
		if(throw_exc) QF_SQL_EXCEPTION(trUtf8("query does not start with SELECT statement\n%1").arg(query_string));
		can_parse = false;
	}
	s = s.slice(String("SELECT").len());

	if(can_parse) do{
		//int ix = 0;
		/*
		if((ix = keyword_pos(s, "FROM")) < 0) {
			//if(throw_exc) QF_SQL_EXCEPTION(trUtf8("query does not contain FROM statement\n%1").arg(query_string));
			break;
		}
		m_queryMap[SelectKey] = s.slice(0, ix).trim();
		s = s.slice(ix + String("FROM").len()).trim();
		*/
		QueryMapKey curr_key = SelectKey;
		foreach(KeyPair kw, keywords) {
			QueryMapKey found_key = InvalidKey;
			int ix = keyword_pos(s, kw.sqlName);
			if(ix < 0) {
				qfDebug() << "\t" << kw.sqlName << ": NOT FOUND";
				continue;
			}
			found_key = kw.key;
#if 0
			{
				QString curr_keyword = "SELECT";
				foreach(KeyPair kw, keywords) {
					if(kw.key == curr_key) {
						curr_keyword = kw.sqlName;
						break;
					}
				}
				qfDebug() << "\t" << curr_keyword << ":" << s.slice(0, ix).trim();
			}
#endif
			m_queryMap[curr_key] = s.slice(0, ix).trim();
			s = s.slice(ix + kw.sqlName.length());
			curr_key = found_key;
		}
		m_queryMap[curr_key] = s.trim();
#if 0
		{
			QString curr_keyword;
			foreach(KeyPair kw, keywords) {
				if(kw.key == curr_key) {
					curr_keyword = kw.sqlName;
					break;
				}
			}
			qfDebug() << "\t" << curr_keyword << ":" << s.trim();
		}
#endif
		qfDebug() << "\treconstructed:" << toString();
		return true;
	} while(false);
	m_queryMap.clear();
	return false;
}
#endif
