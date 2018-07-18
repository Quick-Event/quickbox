#include "querybuilder.h"

#include "connection.h"

#include "../core/log.h"
#include "../core/string.h"

#include <QSqlField>

using namespace qf::core::sql;

//==================================================================
//            QueryBuilder
//==================================================================

const QString QueryBuilder::INNER_JOIN = QStringLiteral("INNER JOIN");
const QString QueryBuilder::LEFT_JOIN = QStringLiteral("LEFT JOIN");

QueryBuilder::QueryBuilder()
{
}

QueryBuilder::~QueryBuilder()
{
}

QString QueryBuilder::buildQuery(const BuildOptions &opts) const
{
	qfLogFuncFrame();
	QString s, qs;
	s = buildSelect(opts);
	if(!s.isEmpty())
		qs = "SELECT" + s;
	s = buildFrom(opts);
	if(!s.isEmpty())
		qs += " FROM" + s;
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
		qs = "(" + qs + ") AS " + m_queryMap[AsKey].toString();
	qfDebug() << "\t" << qs;
	return qs;
}

QString QueryBuilder::buildString(QueryMapKey key) const
{
	QString s = m_queryMap.value(key).toString();
	return s;
}

QString QueryBuilder::buildSelect(const BuildOptions &opts) const
{
	QStringList select_fields;
	QVariant v = m_queryMap.value(SelectKey);
	const SelectTableFieldsList lst = v.value<SelectTableFieldsList>();
	for(const SelectTableFields &flds : lst) {
		if(flds.table.isEmpty()) {
			select_fields << flds.fields;
		}
		else {
			bool is_mangle_table_name = isMangleLongFieldNames(opts);
			QStringList fldlst;
			if(flds.fields.compare(QLatin1String("*")) == 0 && is_mangle_table_name) {
				fldlst = fieldsForTable(opts.connectionName(), flds.table);
			}
			else {
				fldlst = qf::core::String(flds.fields).splitAndTrim(',');
			}
			Q_FOREACH(const QString &orig_fld_name, fldlst) {
				QString new_fld_name = flds.table + '.' + orig_fld_name;
				if(is_mangle_table_name) {
					QString as = QStringLiteral(" AS ");
					if(!orig_fld_name.contains(as, Qt::CaseInsensitive))
						new_fld_name += as + mangleLongFieldName(flds.table, orig_fld_name);
				}
				select_fields << new_fld_name;
			}
		}
	}
	QString ret = m_queryMap.value(SelectFlagsKey).toString();
	ret += ' ' + select_fields.join(QStringLiteral(", "));
	return ret;
}

QString QueryBuilder::JoinDefinition::buildString(const QueryBuilder::BuildOptions &opts) const
{
	QString relation;
	if(joinRelation.isValid()) {
		if(joinRelation.userType() == qMetaTypeId<QueryBuilder>()) {
			QueryBuilder qb = joinRelation.value<QueryBuilder>();
			relation = qb.toString(opts);
		}
		else {
			relation = joinRelation.toString();
		}
	}
	if(relation.isEmpty()) {
		relation = slaveKey.table;
		if(!slaveKey.tableAlias.isEmpty())
			relation += " AS " + slaveKey.tableAlias;
	}
	QString ret;
	if(masterKey.isEmpty() && slaveKey.isEmpty()) {
		ret += relation;
	}
	else {
		ret += joinKind;
		ret += ' ' + relation;
		ret += " ON " + slaveKey.buildString() + '=' + masterKey.buildString();
		if(!joinRestriction.isEmpty())
			ret += " AND (" + joinRestriction + ")";
	}
	return ret;
}

QStringList QueryBuilder::fieldsForTable(const QString &connection_name, const QString &table_name) const
{
	qfLogFuncFrame() << "connection name:" << connection_name;
	QStringList ret;
	QSqlDatabase db = QSqlDatabase::database(connection_name);
	if(db.isValid()) {
		qf::core::sql::Connection conn = qf::core::sql::Connection(db);
		QSqlRecord rec = conn.record(table_name);
		for(int i=0; i<rec.count(); i++) {
			QSqlField fld = rec.field(i);
			ret << fld.name();
		}
	}
	if(ret.isEmpty()) {
		qfWarning() << "Cannot retrieve fields for table:" << table_name;
	}
	return ret;
}

QString QueryBuilder::buildFrom(const QueryBuilder::BuildOptions &opts) const
{
	QString ret;
	QVariant v = m_queryMap.value(FromKey);
	const JoinDefinitionList lst = v.value<JoinDefinitionList>();
	for(const JoinDefinition &join_def : lst) {
		ret += ' ' + join_def.buildString(opts);
	}
	return ret;
}

QString QueryBuilder::buildRest() const
{
	QString s, ret;
	s = buildString(LimitKey);
	if(!s.isEmpty()) ret += " LIMIT " + s;
	s = buildString(OffsetKey);
	if(!s.isEmpty()) ret += " OFFSET " + s;
	return ret;
}

void QueryBuilder::addSelect(const QueryBuilder::SelectTableFields &sf, const QString &flags)
{
	if(!flags.isEmpty())
		m_queryMap[SelectFlagsKey] = flags;
	QVariant v = m_queryMap.value(SelectKey);
	SelectTableFieldsList lst = v.value<SelectTableFieldsList>();
	lst << sf;
	v = QVariant::fromValue(lst);
	m_queryMap[SelectKey] = v;
}

QueryBuilder& QueryBuilder::select(const QString &fields, const QString &flags)
{
	addSelect(SelectTableFields(QString(), fields), flags);
	return *this;
}

QueryBuilder & QueryBuilder::select2(const QString & table_name, const QString & fields, const QString & flags)
{
	addSelect(SelectTableFields(table_name, fields), flags);
	return *this;
}

QueryBuilder& QueryBuilder::from(const QString &table_name)
{
	qfLogFuncFrame();
	addJoin(JoinDefinition(QString(), QString(), table_name.trimmed(), QString(), QString()));
	return *this;
}

QueryBuilder& QueryBuilder::from(const QueryBuilder &table)
{
	qfLogFuncFrame();
	addJoin(JoinDefinition(TableKey(), TableKey(), QVariant::fromValue(table), QString(), QString()));
	return *this;
}

QueryBuilder& QueryBuilder::join(const QString &t1_key, const QString &t2_key, const QString &join_kind)
{
	return joinRestricted(t1_key, t2_key, QString(), join_kind);
}

QueryBuilder& QueryBuilder::join(const QString &join)
{
	qfLogFuncFrame();
	addJoin(JoinDefinition(TableKey(), TableKey(), join, QString(), QString()));
	return *this;
}

QueryBuilder& QueryBuilder::joinQuery(const QString &t1_key, const QString &t2_select_query, const QString &t2_key_field, const QString  &join_kind)
{
	return joinQueryRestricted(t1_key, t2_select_query, t2_key_field, QString(), join_kind);
}

QueryBuilder& QueryBuilder::joinQuery(const QString &t1_key, const QueryBuilder &t2_select_query, const QString &t2_key_field, const QString  &join_kind)
{
	return joinQueryRestricted(t1_key, t2_select_query, t2_key_field, QString(), join_kind);
}

QueryBuilder& QueryBuilder::joinRestricted(const QString& t1_key, const QString& t2_key, const QString& where_restriction, const QString  &join_kind)
{
	qfLogFuncFrame();
	addJoin(JoinDefinition(t1_key, t2_key, QString(), join_kind, where_restriction));
	return *this;
}

QueryBuilder& QueryBuilder::joinQueryRestricted(const QString &t1_key, const QString &t2_select_query, const QString &t2_key_field, const QString  &where_restriction, const QString  &join_kind)
{
	qfLogFuncFrame();
	QString t2_name = t2_select_query.trimmed();
	int ix = t2_name.lastIndexOf(' ');
	if(ix > 0) {
		t2_name = t2_name.mid(ix+1);
	}
	addJoin(JoinDefinition(TableKey(t1_key), TableKey(t2_name, t2_key_field), t2_select_query, join_kind, where_restriction));
	return *this;
}

QueryBuilder &QueryBuilder::joinQueryRestricted(const QString &t1_key, const QueryBuilder &t2_select_query, const QString &t2_key_field, const QString &where_restriction, const QString &join_kind)
{
	qfLogFuncFrame();
	QString t2_name = t2_select_query.buildString(AsKey);
	addJoin(JoinDefinition(TableKey(t1_key), TableKey(t2_name, t2_key_field), QVariant::fromValue(t2_select_query), join_kind, where_restriction));
	return *this;
}

QueryBuilder& QueryBuilder::where(const QString &cond, const QString &oper)
{
	if(!cond.isEmpty()) {
		QString s = m_queryMap.value(WhereKey).toString();
		if(!s.isEmpty())
			s += " " + oper + " ";
		m_queryMap[WhereKey] = s + "(" + cond + ")";
	}
	return *this;
}

QueryBuilder& QueryBuilder::groupBy(const QString &expr)
{
	QString s = m_queryMap.value(GroupByKey).toString();
	if(!s.isEmpty())
		s += " ";
	m_queryMap[GroupByKey] = s + expr;
	return *this;
}

QueryBuilder& QueryBuilder::having(const QString &cond, const QString &oper)
{
	if(!cond.isEmpty()) {
		QString s = m_queryMap.value(HavingKey).toString();
		if(!s.isEmpty())
			s += " " + oper + " ";
		m_queryMap[HavingKey] = s + "(" + cond + ")";
	}
	return *this;
}

QueryBuilder& QueryBuilder::orderBy(const QString &field_and_order_flag)
{
	QString s = m_queryMap.value(OrderByKey).toString();
	if(!s.isEmpty())
		s += ", ";
	m_queryMap[OrderByKey] = s + field_and_order_flag;
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

QVariant QueryBuilder::takeWhere()
{
	QVariant v = take(WhereKey);
	return v;
}

QVariant QueryBuilder::takeOrderBy()
{
	QVariant v = take(OrderByKey);
	return v;
}

QString QueryBuilder::unmangleLongFieldName(const QString &field)
{
	int ix = field.indexOf(mangleSeparator());
	if(ix < 0)
		return field;
	QString ret = field;
	return ret.replace(ix, mangleSeparator().length(), '.');
}

bool QueryBuilder::isMangleLongFieldNames(const QueryBuilder::BuildOptions &opts) const
{
	return !opts.isSqlDriverSupportsTableNamesInSelect();
}

void QueryBuilder::addJoin(const QueryBuilder::JoinDefinition &jd)
{
	qfLogFuncFrame();
	QVariant v = m_queryMap.value(FromKey);
	JoinDefinitionList lst = v.value<JoinDefinitionList>();
	lst << jd;
	v = QVariant::fromValue(lst);
	m_queryMap[FromKey] = v;
}

QVariant QueryBuilder::take(QueryMapKey key)
{
	QVariant ret = m_queryMap.take(key);
	//qfInfo() << __LINE__ << ret;
	//foreach(QueryMapKey i, m_queryMap.keys()) qfInfo() << "\t" << i << ":" << m_queryMap[i];
	return ret;
}

void QueryBuilder::clear()
{
	m_queryMap.clear();
}

bool QueryBuilder::BuildOptions::isSqlDriverSupportsTableNamesInSelect() const
{
	bool ret = false;
	QString connection_name = connectionName();
	QSqlDatabase db = QSqlDatabase::database(connection_name);
	if(db.isValid()) {
		if(db.driverName().startsWith(QLatin1String("QF")))
			ret = true;
	}
	return ret;
}
