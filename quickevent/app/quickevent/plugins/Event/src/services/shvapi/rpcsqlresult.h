#pragma once

#include <shv/chainpack/rpcvalue.h>

#include <QVector>
#include <QVariantList>

namespace shv::chainpack { class RpcResponse; }

class QSqlQuery;
class QSqlRecord;

namespace Event::services::shvapi {

class RpcSqlField
{
public:
	QString name;
	int type = 0;
public:
	//explicit RpcSqlField(const QJsonObject &jo = QJsonObject()) : Super(jo) {}
	shv::chainpack::RpcValue toRpcValue() const;
	QVariant toVariant() const;
	static RpcSqlField fromRpcValue(const shv::chainpack::RpcValue &rv);
	static RpcSqlField fromVariant(const QVariant &v);
};

class RpcSqlResult
{
public:
	int numRowsAffected = 0;
	int lastInsertId = 0;
	QString lastError;
	QVector<RpcSqlField> fields;
	using Row = QVariantList;
	QVariantList rows;
public:
	explicit RpcSqlResult() = default;
	explicit RpcSqlResult(const shv::chainpack::RpcResponse &resp);

	QVariant value(int col, int row) const;

	bool isSelect() const {return !fields.isEmpty();}
	shv::chainpack::RpcValue toRpcValue() const;
	QVariant toVariant() const;
	//static RpcSqlResult fromRpcValue(const shv::chainpack::RpcValue &rv);
	static RpcSqlResult fromVariant(const QVariant &v);
	static RpcSqlResult fromRpcValue(const shv::chainpack::RpcValue &rv);
	static RpcSqlResult fromQuery(QSqlQuery &q);
};

shv::chainpack::RpcValue::Map recordToMap(const QSqlRecord &rec);

}
