#include "utils.h"
#include "log.h"

#include <QString>
#include <QDate>
#include <QRegularExpression>

namespace qf {
namespace core {

const QString& Utils::nullValueString()
{
	static QString n = QStringLiteral("null");
	return n;
}

void qf::core::Utils::parseFieldName(const QString &full_field_name, QString *pfield_name, QString *ptable_name, QString *pdb_name)
{
	QString s = full_field_name;
	QString field_name, table_name, db_name;
	field_name = s;

	int ix = s.lastIndexOf('.');
	if(ix >= 0) {
		field_name = s.mid(ix+1);
		s = s.mid(0, ix);
		table_name = s;

		ix = s.lastIndexOf('.');
		if(ix >= 0) {
			table_name = s.mid(ix+1);
			s = s.mid(0, ix);
			db_name = s;
		}
	}

	if(pfield_name) *pfield_name = field_name;
	if(ptable_name) *ptable_name = table_name;
	if(pdb_name) *pdb_name = db_name;
}

QString Utils::composeFieldName(const QString &field_name, const QString &table_name, const QString &db_name)
{
	QString ret;
	if(!field_name.isEmpty()) {
		ret = field_name;
		if(!table_name.isEmpty()) {
			ret = table_name + '.' + ret;
			if(!db_name.isEmpty()) {
				ret = db_name + '.' + ret;
			}
		}
	}
	return ret;
}

bool Utils::fieldNameEndsWith(const QString &field_name1, const QString &field_name2)
{
	/// psql (podle SQL92) predelava vsechny nazvy sloupcu, pokud nejsou v "" do lowercase, ale mixedcase se lip cte, tak at se to sparuje.
	int l1 = field_name1.length();
	int l2 = field_name2.length();
	if(l2 > l1)
		return false;
	if(field_name1.endsWith(field_name2, Qt::CaseInsensitive)) {
		if(l1 == l2)
			return true; /// same length, must be same
		if(field_name1[l1 - l2 - 1] == '.')
			return true; /// dot '.' is on position l1 - l2 - 1
	}
	return false;
}

bool Utils::fieldNameCmp(const QString &fld_name1, const QString &fld_name2)
{
	if(fld_name1.isEmpty() || fld_name2.isEmpty()) return false;
	if(fieldNameEndsWith(fld_name1, fld_name2)) return true;
	if(fieldNameEndsWith(fld_name2, fld_name1)) return true;
	return false;
}

QVariant Utils::retypeVariant(const QVariant &val, int meta_type_id)
{
	if(meta_type_id == QVariant::Invalid) {
		//qfWarning() << "Cannot convert" << val << "to QVariant::Invalid type!";
		// retype whatever to invalid variant
		return QVariant();
	}
	if(val.userType() == meta_type_id)
		return val;
	if(val.canConvert(meta_type_id)) {
		QVariant ret = val;
		ret.convert(meta_type_id);
		return ret;
	}
	if(meta_type_id < QMetaType::User) {
		if(val.isNull()) {
			QVariant::Type t = (QVariant::Type)meta_type_id;
			return QVariant(t);
		}
	}
	//if(meta_type_id >= QVariant::UserType) {
	//	if(val.userType() >= QVariant::UserType) {
	//		if()
	//	}
	//}
	qfWarning() << "Don't know, how to convert variant type" << val.typeName() << "to:" << meta_type_id << QMetaType::typeName(meta_type_id);
	return val;
}

QVariant Utils::retypeStringValue(const QString &str_val, const QString &type_name)
{
	QByteArray ba = type_name.toLatin1();
	QVariant::Type type = QVariant::nameToType(ba.constData());
	QVariant ret = qf::core::Utils::retypeVariant(str_val, type);
	return ret;
}

int Utils::findCaption(const QString &caption_format, int from_ix, QString *caption)
{
	int ix1 = caption_format.indexOf(QLatin1String("{{"), from_ix);
	if(ix1 >= 0) {
		int ix2 = caption_format.indexOf(QLatin1String("}}"), ix1+2);
		if(ix2 > ix1) {
			if(caption)
				*caption = caption_format.mid(ix1+2, ix2-ix1-2);
			return ix1;
		}
	}
	return -1;
}

QSet<QString> Utils::findCaptions(const QString &caption_format)
{
	QSet<QString> ret;
	QRegExp rx;
	rx.setPattern("\\{\\{([A-Za-z][A-Za-z0-9]*(\\.[A-Za-z][A-Za-z0-9]*)*)\\}\\}");
	rx.setPatternSyntax(QRegExp::RegExp);
	int ix = 0;
	while((ix = rx.indexIn(caption_format, ix)) != -1) {
		ret << rx.cap(1);
		ix += rx.matchedLength();
	}
	return ret;
}

QString Utils::replaceCaptions(const QString format_str, const QString &caption_name, const QVariant &caption_value)
{
	QString ret = format_str;
	QString placeholder = QLatin1String("{{") + caption_name + QLatin1String("}}");
	//qfInfo() << placeholder << "->" << caption_value.toString();
	ret.replace(placeholder, caption_value.toString());
	return ret;
}

QString Utils::replaceCaptions(const QString format_str, const QVariantMap &replacements)
{
	QString ret = format_str;
	QMapIterator<QString, QVariant> it(replacements);
	while(it.hasNext()) {
		it.next();
		ret = replaceCaptions(ret, it.key(), it.value());
	}
	return ret;
}

QString Utils::removeJsonComments(const QString &json_str)
{
	// http://blog.ostermiller.org/find-comment
	QString ret = json_str;
	ret.replace(QRegularExpression("/\\*(?:.|[\\n])*?\\*/"), QString());
	ret.replace(QRegularExpression("(?<!:)//.*[\\n]"), "\n");
	return ret;
}

int Utils::versionStringToInt(const QString &version_string)
{
	int ret = 0;
	for(QString s : version_string.split('.')) {
		int i = s.toInt();
		ret = 100 * ret + i;
	}
	return ret;
}

QString Utils::intToVersionString(int ver)
{
	QString ret;
	while(ver) {
		int i = ver % 100;
		ver /= 100;
		QString s = QString::number(i);
		//if(i < 10 && ver > 0)
		//	s = '0' + s;
		if(ret.isEmpty())
			ret = s;
		else
			ret = s + '.' + ret;
	}
	return ret;
}

bool Utils::invokeMethod_B_V(QObject *obj, const char *method_name)
{
	QVariant ret = false;
	bool ok = QMetaObject::invokeMethod(obj, method_name, Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret));
	if(!ok)
		qfWarning() << obj << "Method" << method_name << "invocation failed!";
	return ret.toBool();
}

QStringList Utils::parseProgramAndArgumentsList(const QString &command_line)
{
	QStringList args;
	QString tmp;
	int quoteCount = 0;
	bool inQuote = false;
	// handle quoting. tokens can be surrounded by double quotes
	// "hello world". three consecutive double quotes represent
	// the quote character itself.
	for (int i = 0; i < command_line.size(); ++i) {
		if (command_line.at(i) == QLatin1Char('"')) {
			++quoteCount;
			if (quoteCount == 3) {
				// third consecutive quote
				quoteCount = 0;
				tmp += command_line.at(i);
			}
			continue;
		}
		if (quoteCount) {
			if (quoteCount == 1)
				inQuote = !inQuote;
			quoteCount = 0;
		}
		if (!inQuote && command_line.at(i).isSpace()) {
			if (!tmp.isEmpty()) {
				args += tmp;
				tmp.clear();
			}
		} else {
			tmp += command_line.at(i);
		}
	}
	if (!tmp.isEmpty())
		args += tmp;
	return args;
}

}}
