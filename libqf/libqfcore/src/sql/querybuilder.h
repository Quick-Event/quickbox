#ifndef QF_CORE_SQL_QUERYBUILDER_H
#define QF_CORE_SQL_QUERYBUILDER_H

#include "../core/coreglobal.h"
#include "../core/utils.h"

#include <QSqlDatabase>
#include <QMap>
#include <QVariant>

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT QueryBuilder
{
	//Q_DECLARE_TR_FUNCTIONS(QueryBuilder);
public:
	class QFCORE_DECL_EXPORT BuildOptions
	{
	public:
		BuildOptions() : m_connectionName(QSqlDatabase::defaultConnection) {}

		QString connectionName() const {return m_connectionName;}
		void setConnectionName(const QString &val) {m_connectionName = val;}

		bool isSqlDriverSupportsTableNamesInSelect() const;
	private:
		QString m_connectionName;
	};
public:
	QueryBuilder();
	virtual ~QueryBuilder();
public:
	QString toString(const BuildOptions &opts = BuildOptions()) const {return buildQuery(opts);}
	bool isEmpty() const {return m_queryMap.isEmpty();}
	void clear();
	/**
	* Example:
			\code
		QueryBuilder qb;
		qb.select("zakazky.*, jmena.psc", "distinct").select("jmena.name")
			.from("zakazky")
			.join("zakaznikID", "kontakty AS jmena", "id")
			.where("zakazky.id = 11 AND vystavilDne > '2006-01-01'")
			.where("AND jmena.zatrideni = 1")
			.orderBy(zakazky.cislo)
			.limit(1);
		\endcode
		or
		\code
		QueryBuilder qb_inner;
		qb_inner.select("*").from("kontakty")
			.where("zatrideni = 1")
			.as(jmena);
		QueryBuilder qb;
		qb.select("zakazky.*, jmena.psc", "distinct").select("jmena.name")
			.from("zakazky")
			.join("zakaznikID", qb_inner, "id")
			.where("zakazky.id = 11 AND vystavilDne > '2006-01-01'")
			.where("jmena.zatrideni = 1")
			.orderBy(zakazky.cislo)
			.limit(1);
		\endcode
	* @param flags  Can be [ DISTINCT | ALL | DITINCT ON expr ]
	*/
	QueryBuilder& select(const QString &fields, const QString &flags = QString());
	//! Same as select(), but it prepends table_name before each field.
	QueryBuilder& select2(const QString &table_name, const QString &fields, const QString &flags = QString());
	/**
	* @param table_name For details see join() parameter \a t2_name .
	*/
	QueryBuilder& from(const QString &table_name);
	QueryBuilder& from(const QueryBuilder &table);

	//! @param t1_key in form tablename.keyname.
	//! @param t2_key in form tablename.keyname.
	QueryBuilder& join(const QString &t1_key, const QString &t2_key, const QString  &join_kind = "LEFT JOIN");
	//! Tady si muzu do joinu napsat co chci (vcetne join_kind), nekdy to jinak nejde.
	QueryBuilder& join(const QString &join);
	/**
	*  Join table to query.
	* @param t1_key in form tablename.keyname.
	* @param t2_name Can be table name or SELECT ... . In both cases a last word is considered to be a tablename.
	*/
	QueryBuilder& joinQuery(const QString &t1_key, const QString &t2_select_query, const QString &t2_key_field, const QString  &join_kind = "LEFT JOIN");
	QueryBuilder& joinQuery(const QString &t1_key, const QueryBuilder &t2_select_query, const QString &t2_key_field, const QString  &join_kind = "LEFT JOIN");

	/// @param where_restriction se prida pomoci logickeho operatoru AND k ON klauzuli JOINu
	QueryBuilder& joinRestricted(const QString &t1_key, const QString &t2_key, const QString &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN");
	QueryBuilder& joinQueryRestricted(const QString &t1_key, const QString &t2_select_query, const QString &t2_key_field, const QString  &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN");
	QueryBuilder& joinQueryRestricted(const QString &t1_key, const QueryBuilder &t2_select_query, const QString &t2_key_field, const QString  &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN");
	/**
	* @param oper Condition can be chained using multiple calls of where(), than \a oper is used to join logical conditions.
	*/
	QueryBuilder& where(const QString &cond, const QString &oper = "AND");
	QueryBuilder& groupBy(const QString &expr);
	QueryBuilder& having(const QString &cond, const QString &oper = "AND");
	/**
	* @param field_and_order_flag order flag can be omited, in such a case ascending order is considered.
	*/
	QueryBuilder& orderBy(const QString &field_and_order_flag);
	QueryBuilder& limit(int n);
	QueryBuilder& offset(int n);
	QueryBuilder& as(const QString &alias_name);

	QueryBuilder& clearWhere();

	static QString mangleSeparator() {static QString ret = QStringLiteral("__"); return ret;}
	static QString mangleLongFieldName(const QString &table, const QString &field) {return table + mangleSeparator() + field;}
	static QString unmangleLongFieldName(const QString &field) {
		int ix = field.indexOf(mangleSeparator());
		if(ix < 0)
			return field;
		QString ret = field;
		return ret.replace(ix, mangleSeparator().length(), '.');
	}
private:
	enum QueryMapKey {InvalidKey = 0, SelectKey, SelectFlagsKey, FromKey, WhereKey, GroupByKey, HavingKey, OrderByKey, LimitKey, OffsetKey, AsKey};
	typedef QMap<QueryMapKey, QVariant> QueryMap;
	struct TableKey
	{
		QString table;
		QString field;

		TableKey(const QString &t, const QString &f) : table(t), field(f) {}
		TableKey(const QString &n = QString())
		{
			int ix = n.lastIndexOf('.');
			if(ix > 0) {
				table = n.mid(0, ix).trimmed();
				field = n.mid(ix+1).trimmed();
			}
			else {
				field = n;
			}
		}

		bool isEmpty() const {return table.isEmpty() && field.isEmpty();}
		QString buildString() const {return table + '.' + field;}
	};
	struct SelectTableFields {
		QString table;
		QString fields;
		SelectTableFields(const QString &t, const QString &f) : table(t), fields(f) {}
	};
	struct JoinDefinition {
		TableKey masterKey;
		TableKey slaveKey;
		QVariant joinRelation;
		QString joinKind;
		QString joinRestriction;

		JoinDefinition(const TableKey &tk1, const TableKey &tk2, const QVariant &rel, const QString &kind, const QString &restr)
			: masterKey(tk1), slaveKey(tk2), joinRelation(rel), joinKind(kind), joinRestriction(restr) {}

		QString buildString(const BuildOptions &opts) const;
	};
public:
	typedef QList<SelectTableFields> SelectTableFieldsList;
	typedef QList<JoinDefinition> JoinDefinitionList;
private:
	bool isMangleLongFieldNames(const BuildOptions &opts) const;
	QStringList fieldsForTable(const QString &connection_name, const QString &table_name) const;
	void addSelect(const SelectTableFields &sf, const QString &flags);
	void addJoin(const JoinDefinition &jd);
	//! Removes part of query with key \a key . Returns removed part.
	QVariant take(QueryMapKey key);

	QString buildSelect(const BuildOptions &opts) const;
	QString buildFrom(const BuildOptions &opts)const;
	QString buildWhere() const {return buildString(WhereKey);}
	QString buildGroupBy() const {return buildString(GroupByKey);}
	QString buildHaving() const {return buildString(HavingKey);}
	QString buildOrderBy() const {return buildString(OrderByKey);}
	//!       Build LIMIT and OFFSET clauses.
	QString buildRest() const;
	QString buildQuery(const BuildOptions &opts) const;
	QString buildString(QueryMapKey key) const;
private:
	QueryMap m_queryMap;
};

}}}

Q_DECLARE_METATYPE(qf::core::sql::QueryBuilder)
Q_DECLARE_METATYPE(qf::core::sql::QueryBuilder::SelectTableFieldsList)
Q_DECLARE_METATYPE(qf::core::sql::QueryBuilder::JoinDefinitionList)

#endif // QF_CORE_SQL_QUERYBUILDER_H

