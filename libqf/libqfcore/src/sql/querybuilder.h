#ifndef QF_CORE_SQL_QUERYBUILDER_H
#define QF_CORE_SQL_QUERYBUILDER_H

#include "../core/coreglobal.h"

#include <QMap>
#include <QString>

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT QueryBuilder
{
	//Q_DECLARE_TR_FUNCTIONS(QueryBuilder);
public:
	enum QueryMapKey {InvalidKey = 0, SelectKey, SelectFlagsKey, FromKey, WhereKey, GroupByKey, HavingKey, OrderByKey, LimitKey, OffsetKey, AsKey};
public:
	QueryBuilder();
	virtual ~QueryBuilder();
public:
	QString toString() const {return buildQuery();}
	//! Removes part of query with key \a key . Returns removed part.
	QString take(QueryMapKey key);
	bool isEmpty() const {return m_queryMap.isEmpty();}

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
	//! Jako select(const QString &fields, const QString &flags = QString()), ale pred kazdy field z fields prida table_name a tecku.
	QueryBuilder& select2(const QString &table_name, const QString &fields, const QString &flags = QString());
	/**
	* @param table_name For details see join() parameter \a t2_name .
	*/
	QueryBuilder& from(const QString &table_name);
	QueryBuilder& from(const QueryBuilder &table);
	/**
	*  Join table to query.
	* @param t1_key Can be in form tablename.keyname.
	* @param t2_name Can be table name or SELECT ... . In both cases a last word is considered to be a tablename.
	*/
	QueryBuilder& join2(const QString &t1_key, const QString &t2_name, const QString &t2_key, const QString  &join_kind = "LEFT JOIN");
	//! @param t2_key in form tablename.keyname.
	//! See \a join(const QString &t1_key, const QString &t2_name, const QString &t2_key, const QString  &join_kind) .
	QueryBuilder& join(const QString &t1_key, const QString &t2_key, const QString  &join_kind = "LEFT JOIN");
	/// @param where_restriction se prida pomoci logickeho operatoru AND k ON klauzuli JOINu
	QueryBuilder& joinRestricted(const QString &t1_key, const QString &t2_key, const QString &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN");
	QueryBuilder& joinRestrictedAliased(const QString &t1_key, const QString &t2_name, const QString &t2_key, const QString &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN");
	//! @param t1_key in form tablename.keyname or keyname, in such a case recentlyMentionedRelation is considered to be a tablename.
	QueryBuilder& join(const QString &t1_key, const QueryBuilder &t2, const QString &t2_key, const QString  &join_kind = "LEFT JOIN");
	//! Tady si muzu do joinu napsat co chci (vcetne join_kind), nekdy to jinak nejde.
	QueryBuilder& join(const QString &join);
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
private:
	typedef QMap<QueryMapKey, QString> QueryMap;
private:
	QString buildSelect() const;
	QString buildFrom() const {return build(FromKey);}
	QString buildWhere() const {return build(WhereKey);}
	QString buildGroupBy() const {return build(GroupByKey);}
	QString buildHaving() const {return build(HavingKey);}
	QString buildOrderBy() const {return build(OrderByKey);}
	//!       Build LIMIT and OFFSET clauses.
	QString buildRest() const;
	QString buildQuery() const;
	QString build(QueryMapKey key) const;
private:
	QueryMap m_queryMap;
};

}}}

#endif // QF_CORE_SQL_QUERYBUILDER_H

