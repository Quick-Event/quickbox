#include "dbenum.h"

#include "query.h"
#include "../core/utils.h"
#include "../core/log.h"

#include <QSqlRecord>
//#include <QSqlError>
#include <QJsonDocument>

using namespace qf::core::sql;

//============================================================
//                              DbEnum
//============================================================
QMap<QString, int> DbEnum::s_fieldMapping;

DbEnum::DbEnum(int id, const QString & group_name, const QString & group_id, const QString & caption)
{
	setId(id);
	setGroupName(group_name);
	setGroupId(group_id);
	setCaption(caption);
}

QString DbEnum::caption() const
{
	QString ret = m_values.value(FieldCaption).toString();
	if(ret.isEmpty())
		ret = groupId();
	return ret;
}

DbEnum::DbEnum(const Query &q)
{
	qfLogFuncFrame();
	QSqlRecord rec = q.record();
	qfDebug() << "\t" << rec.value(0).toString() << rec.value(1).toString();
	if(s_fieldMapping.isEmpty()) {
		for(int i=0; i<rec.count(); i++) {
			QString fld_name = rec.fieldName(i);
			if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("id")))
				s_fieldMapping[fld_name] = FieldId;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("groupName")))
				s_fieldMapping[fld_name] = FieldGroupName;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("groupId")))
				s_fieldMapping[fld_name] = FieldGroupId;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("pos")))
				s_fieldMapping[fld_name] = FieldPos;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("abbreviation")))
				s_fieldMapping[fld_name] = FieldAbbreviation;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("value")))
				s_fieldMapping[fld_name] = FieldValue;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("caption")))
				s_fieldMapping[fld_name] = FieldCaption;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("userText")))
				s_fieldMapping[fld_name] = FieldUserText;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("grants")))
				s_fieldMapping[fld_name] = FieldGrants;
		}
	}
	for(int i=0; i<LastFieldIndex; i++)
		m_values << QVariant();
	for(int i=0; i<rec.count(); i++) {
		qfDebug() << "\t" << i << "field:" << rec.fieldName(i);
		QString fld_name = rec.fieldName(i);
		int fld_ix = s_fieldMapping.value(fld_name, -1);
		if(fld_ix < 0)
			continue;
		QVariant v = rec.value(i);
		if(fld_ix == FieldValue) {
			QString s = v.toString().trimmed();
			if(!s.isEmpty()) {
				QByteArray ba = s.toUtf8();
				QJsonParseError err;
				QJsonDocument jsd = QJsonDocument::fromJson(ba, &err);
				if(err.error == QJsonParseError::NoError) {
					v = jsd.toVariant();
				}
				else {
					qfError() << "DbEnum parse value ERROR:" << s << "\n" << err.errorString();
				}
			}
		}
		m_values[fld_ix] = v;
	}
	/*
	{
		QString localized_caption = caption();
		if(localized_caption.isEmpty()) localized_caption = groupId();
		{
			/// pokud existuje lokalizace v datech ma prednost
			QFDataTranslator *dtr = dataTranslator();
			if(dtr) {
				QString domain = "enumz." + groupName();
				QString src = groupId();
				QString lc_caption = dtr->translate(src, domain);
				if(lc_caption != src) localized_caption = lc_caption;
			}
		}
		values[FieldCaption] = localized_caption;
	}
	{
		QString localized_abbreviation = abbreviation();
		//if(localized_addreviation.isEmpty()) localized_addreviation = groupId()%":abbr";
		{
			/// pokud existuje lokalizace v datech ma prednost
			QFDataTranslator *dtr = dataTranslator();
			if(dtr) {
				QString domain = "enumz." + groupName();
				QString src = groupId()%":abbr";
				QString lc_abbr = dtr->translate(src, domain);
				if(lc_abbr != src) localized_abbreviation = lc_abbr;
			}
		}
		values[FieldAbbreviation] = localized_abbreviation;
	}
	*/
}

void DbEnum::setValue(DbEnum::FieldIndexes ix, const QVariant & val)
{
	if(m_values.isEmpty())
		for(int i=0; i<LastFieldIndex; i++)
			m_values << QVariant();
	m_values[ix] = val;
}

QString DbEnum::fillInPlaceholders(const QString& text_with_placeholders) const
{
	QString ret = text_with_placeholders;
	ret.replace("${groupName}", groupName());
	ret.replace("${groupId}", groupId());
	ret.replace("${pos}", QString::number(pos()));
	ret.replace("${abbreviation}", abbreviation());
	ret.replace("${value}", value().toString());
	ret.replace("${caption}", caption());
	return ret;
}

/*
QFDataTranslator* DbEnum::dataTranslator() const
{
	QFDataTranslator *ret = NULL;
	{
		QFAppDataTranslatorInterface *appi = dynamic_cast<QFAppDataTranslatorInterface *>(QCoreApplication::instance());
		if(appi) {
			ret = appi->dataTranslator();
		}
	}
	return ret;
}

QString DbEnum::localizedCaption() const
{
	QString ret = caption();
	QFDataTranslator *dtr = dataTranslator();
	if(dtr) ret = dtr->translate(ret, "enumz."%groupName());
	return ret;
}

QString DbEnum::localizedAbbreviation() const
{
	QString ret = abbreviation();
	QFDataTranslator *dtr = dataTranslator();
	if(dtr) ret = dtr->translate(ret, "enumz."%groupName());
	return ret;
}
*/
