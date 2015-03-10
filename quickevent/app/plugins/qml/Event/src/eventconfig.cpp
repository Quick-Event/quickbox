#include "eventconfig.h"

#include <qf/core/assert.h>
#include <qf/core/utils.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>

static QVariant retypeStringValue(const QString &str_val, const QString &type_name)
{
	QByteArray ba = type_name.toLatin1();
	QVariant::Type type = QVariant::nameToType(ba.constData());
	QVariant ret = qf::core::Utils::retypeVariant(str_val, type);
	return ret;
}

EventConfig::EventConfig(QObject *parent)
	: QObject(parent)
{
}

void EventConfig::setValues(const QVariantMap &vals)
{
	m_data.clear();
	QMapIterator<QString, QVariant> it(vals);
	while(it.hasNext()) {
		it.next();
		QString key = it.key();
		if(knownKeys().contains(key)) {
			m_data[key] = it.value();
		}
	}
}

QVariant EventConfig::value(const QString &key, const QVariant &default_value)
{
	QF_ASSERT(knownKeys().contains(key), "Key " + key + " is not known key!", return QVariant());
	return m_data.value(key, default_value);
}

void EventConfig::setValue(const QString &key, const QVariant &val)
{
	QF_ASSERT(knownKeys().contains(key), "Key " + key + " is not known key!", return);
	m_data[key] = val;
}

void EventConfig::load()
{
	using namespace qf::core::sql;
	Connection conn = Connection::forName();
	Query q(conn);
	QueryBuilder qb;
	qb.select("ckey, cvalue, ctype").from("config");
	if(q.exec(qb.toString())) while(q.next()) {
		QString key = q.value(0).toString();
		if(knownKeys().contains(key)) {
			QVariant val = q.value(1);
			QString type = q.value(2).toString();
			m_data[key] = retypeStringValue(val.toString(), type);
		}
	}
}

void EventConfig::save()
{
	using namespace qf::core::sql;
	Connection conn = Connection::forName();
	Query q(conn);
	QMapIterator<QString, QVariant> it(m_data);
	while(it.hasNext()) {
		it.next();
		QString key = it.key();
		QVariant val = m_data[key];
		q.prepare("UPDATE config SET cvalue=:val WHERE ckey=:key");
		q.bindValue(":key", key);
		q.bindValue(":val", val);
		q.exec();
		if(q.numRowsAffected() < 1) {
			QString type = val.typeName();
			q.prepare("INSERT INTO config (ckey, cvalue, ctype) VALUES (:key, :val, :type)");
			q.bindValue(":key", key);
			q.bindValue(":type", type);
			q.bindValue(":val", val);
			q.exec();
		}
	}
}

const QSet<QString> &EventConfig::knownKeys()
{
	static QSet<QString> s;
	if(s.isEmpty()) {
		s << "event.stageCount"
		  << "event.name"
		  << "event.date"
		  << "event.description"
		  << "event.place"
		  << "event.mainReferee"
		  << "event.director"
		  << "event.importId";
	}
	return s;
}

