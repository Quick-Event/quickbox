//
// Author: Frantisek Vacek <fanda.vacek@gmail.com>, (C) 2004,2015
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_CORE_SQL_DBENUM_H
#define QF_CORE_SQL_DBENUM_H

#include "../core/coreglobal.h"

#include <QVariant>
#include <QMap>

namespace qf {
namespace core {
namespace sql {

class Query;

class QFCORE_DECL_EXPORT DbEnum
{
public:
	enum FieldIndexes {FieldId = 0, FieldGroupName, FieldGroupId, FieldPos, FieldAbbreviation, FieldValue, FieldCaption, FieldUserText, FieldGrants, LastFieldIndex};
public:
	DbEnum() {}
	DbEnum(const Query &q);
	DbEnum(int id, const QString &group_name, const QString &group_id, const QString &caption);
public:
	int id() const {return m_values.value(FieldId).toInt();}
	void setId(int id) {setValue(FieldId, id);}
	QString groupName() const {return m_values.value(FieldGroupName).toString();}
	void setGroupName(const QString &s) {setValue(FieldGroupName, s);}
	QString groupId() const {return m_values.value(FieldGroupId).toString();}
	void setGroupId(const QString &s) {setValue(FieldGroupId, s);}
	int pos() const {return m_values.value(FieldPos).toInt();}
	QString abbreviation() const {return m_values.value(FieldAbbreviation).toString();}
	//QString localizedAbbreviation() const;
	QVariant value() const {return m_values.value(FieldValue);}
	QString caption() const {return m_values.value(FieldCaption).toString();}
	//QString localizedCaption() const;
	void setCaption(const QString &s) {setValue(FieldCaption, s);}
	QString userText() const {return m_values.value(FieldUserText).toString();}
	void setUserText(const QString &s) {setValue(FieldUserText, s);}
	bool isValid() const {return !m_values.isEmpty();}
	QString grants() const {return m_values.value(FieldGrants).toString();}
	QString fillInPlaceholders(const QString &text_with_placeholders) const;

	bool operator==(const DbEnum &o) const {return groupId()==o.groupId() && groupName()==o.groupName();}
private:
	void setValue(FieldIndexes ix, const QVariant &val);
private:
	QVariantList m_values;
	static QMap<QString, int> s_fieldMapping;
};

class QFCORE_DECL_EXPORT DbEnumCache
{
public:
	DbEnumCache(const QString connection_name = QString()) : m_connectionName(connection_name) {}
	virtual ~DbEnumCache() {}
public:
	struct QFCORE_DECL_EXPORT EnumList : public QList<DbEnum>
	{
		int indexOf(const QString &group_id) const;
		DbEnum valueForId(const QString &group_id) const;
	};
public:
	EnumList dbEnumsForGroup(const QString &group_name);
	DbEnum dbEnum(const QString &group_name, const QString &group_id);

	bool isEmpty() const {return m_enumCache.isEmpty();}
	virtual void reload(const QString &group_name);
	//! delete all groups wen group_name is empty
	void clear(const QString &group_name = QString());
	/// ensure that group_name enums are loaded in the cache
	void ensure(const QString & group_name);
private:
	QString m_connectionName;
	QMap<QString, EnumList> m_enumCache;
};

}}}

#endif // QF_CORE_SQL_DBENUM_H

