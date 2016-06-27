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
#include <QVector>

namespace qf {
namespace core {
namespace sql {

class Query;

class QFCORE_DECL_EXPORT DbEnum
{
public:
	enum FieldIndexes {FieldId = 0, FieldGroupName, FieldGroupId, FieldPos, FieldAbbreviation, FieldValue, FieldCaption, FieldColor, FieldGrants, LastFieldIndex};
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
	QString caption() const;
	//QString localizedCaption() const;
	void setCaption(const QString &s) {setValue(FieldCaption, s);}
	QColor color() const;
	void setColor(const QString &s);
	void setColor(const QColor &c);
	bool isValid() const {return !m_values.isEmpty();}
	QString grants() const {return m_values.value(FieldGrants).toString();}
	QString fillInPlaceholders(const QString &text_with_placeholders) const;

	bool operator==(const DbEnum &o) const {return groupId()==o.groupId() && groupName()==o.groupName();}
private:
	void setValue(FieldIndexes ix, const QVariant &val);
private:
	QVector<QVariant> m_values;
	static QMap<QString, int> s_fieldMapping;
};

}}}

Q_DECLARE_METATYPE(qf::core::sql::DbEnum)

#endif // QF_CORE_SQL_DBENUM_H

