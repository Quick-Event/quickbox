#include "eventconfig.h"

#include <qf/core/assert.h>
#include <qf/core/utils.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>

#include <QDateTime>

using namespace Event;

namespace {
static const auto EVENT_NAME = QStringLiteral("event.name");
}

EventConfig::EventConfig(QObject *parent)
	: QObject(parent)
{
}
/*
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
*/
QVariant EventConfig::value(const QStringList &path, const QVariant &default_value) const
{
	//QF_ASSERT(knownKeys().contains(key), "Key " + key + " is not known key!", return QVariant());
	QVariant ret = default_value;
	if(!path.isEmpty()) {
		QVariantMap m = m_data;
		for (int i = 0; i < path.count() - 1; ++i) {
			const QString &key = path[i];
			m = m.value(key).toMap();
		}
		ret = m.value(path.last(), default_value);
	}
	return ret;
}

void EventConfig::setValue(const QStringList &path, const QVariant &val)
{
	QF_ASSERT(!path.isEmpty(), "Empty path!", return);
	m_data = setValue_helper(m_data, path, val);
}

void EventConfig::load()
{
	using namespace qf::core::sql;
	Connection conn = Connection::forName();
	Query q(conn);
	QueryBuilder qb;
	qb.select("ckey, cvalue, ctype").from("config").orderBy("ckey");
	if(q.exec(qb.toString())) while(q.next()) {
		QString key = q.value(0).toString();
		/*
		if(!knownKeys().contains(key)) {
			qfWarning() << "Config key" << key << "is not known to the QuickEvent config system";
		}
		*/
		QVariant val = q.value(1);
		QString type = q.value(2).toString();
		QVariant v = qf::core::Utils::retypeStringValue(val.toString(), type);
		setValue(key, v);
	}
}

void EventConfig::save(const QString &path_to_save)
{
	QVariantMap m;
	save_helper(m, QString(), m_data);
	using namespace qf::core::sql;
	Connection conn = Connection::forName();
	Query q_up(conn);
	q_up.prepare("UPDATE config SET cvalue=:val WHERE ckey=:key", qf::core::Exception::Throw);
	Query q_ins(conn);
	q_ins.prepare("INSERT INTO config (ckey, cvalue, ctype) VALUES (:key, :val, :type)", qf::core::Exception::Throw);
	QMapIterator<QString, QVariant> it(m);
	while(it.hasNext()) {
		it.next();
		QString key = it.key();
		if(!path_to_save.isEmpty()) {
			if(!key.startsWith(path_to_save))
				continue;
			if(key.length() > path_to_save.length() && key[path_to_save.length()] != '.')
				continue;
		}
		QVariant val = it.value();
		QString val_str;
		if(val.type() == QVariant::Date)
			val_str = val.toDate().toString(Qt::ISODate);
		else if(val.type() == QVariant::Time)
			val_str = val.toTime().toString(Qt::ISODate);
		else if(val.type() == QVariant::DateTime)
			val_str = val.toDateTime().toString(Qt::ISODate);
		else
			val_str = val.toString();
		q_up.bindValue(":key", key);
		q_up.bindValue(":val", val);
		q_up.exec();
		if(q_up.numRowsAffected() < 1) {
			QString type = val.typeName();
			q_ins.bindValue(":key", key);
			q_ins.bindValue(":type", type);
			q_ins.bindValue(":val", val);
			q_ins.exec();
		}
	}
}

void EventConfig::save_helper(QVariantMap &ret, const QString &current_path, const QVariant &val)
{
	if(val.type() == QVariant::Map) {
		QVariantMap m = val.toMap();
		QMapIterator<QString, QVariant> it(m);
		while(it.hasNext()) {
			it.next();
			QString cp = it.key();
			if(!current_path.isEmpty())
				cp = current_path + '.' + cp;
			save_helper(ret, cp, it.value());
		}
	}
	else {
		ret[current_path] = val;
	}
}

QVariantMap EventConfig::setValue_helper(const QVariantMap &m, const QStringList &path, const QVariant &val)
{
	QVariantMap ret;
	QF_ASSERT(!path.isEmpty(), "Empty path!", return ret);
	if(path.count() == 1) {
		ret = m;
		ret[path.first()] = val;
	}
	else {
		QStringList p = path;
		QString key = p.takeFirst();
		ret = m;
		ret[key] = setValue_helper(m.value(key).toMap(), p, val);
	}
	return ret;
}

int EventConfig::stageCount() const
{
	return value(QStringLiteral("event.stageCount")).toInt();
}

int EventConfig::currentStageId() const
{
	int ret = value(QStringLiteral("event.currentStageId")).toInt();
	if(ret == 0)
		ret = 1;
	return ret;
}

int EventConfig::handicapLength() const
{
	return value(QStringLiteral("event.handicapLength")).toInt();
}

QString EventConfig::director() const
{
	return value(QStringLiteral("event.director")).toString();
}

QString EventConfig::mainReferee() const
{
	return value(QStringLiteral("event.mainReferee")).toString();
}
/*
const QSet<QString> &EventConfig::knownKeys()
{
	static QSet<QString> s;
	if(s.isEmpty()) {
		s << "event.stageCount"
		  << EVENT_NAME
		  << "event.date"
		  << "event.description"
		  << "event.place"
		  << "event.mainReferee"
		  << "event.director"
		  << "event.importId";
	}
	return s;
}
*/

