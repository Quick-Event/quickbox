#ifndef CLIOPTIONS_H
#define CLIOPTIONS_H

#include "../core/coreglobal.h"
#include "../core/exception.h"

#include <QObject>
#include <QVariantMap>
#include <QSharedData>
#include <QStringList>

#if defined(_MSC_VER)
// disable throw() warning for MS VC
#pragma warning( disable : 4290 )
#endif

class QTextStream;

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT CLIOptions : public QObject
{
	Q_OBJECT
public:
	CLIOptions(QObject *parent = NULL);
	virtual ~CLIOptions();
public:
	class QFCORE_DECL_EXPORT Option
	{
	private:
		struct Data : public QSharedData
		{
			QVariant::Type type;
			QStringList names;
			QVariant value;
			QVariant defaultValue;
			QString comment;
			bool mandatory;

			Data(QVariant::Type type = QVariant::Invalid) : type(type), mandatory(false) {}
		};
		QSharedDataPointer<Data> d;

		class NullConstructor {};
		static const Option& sharedNull();
		Option(NullConstructor);
	public:
		bool isNull() const {return d == sharedNull().d;}

		Option& setNames(const QStringList &names) {d->names = names; return *this;}
		Option& setNames(const QString &name) {d->names = QStringList() << name; return *this;}
		Option& setNames(const QString &name1, const QString name2) {d->names = QStringList() << name1 << name2; return *this;}
		QStringList names() const {return d->names;}
		Option& setType(QVariant::Type type) {d->type = type; return *this;}
		QVariant::Type type() const {return d->type;}
		Option& setValueString(const QString &val);
		Option& setValue(const QVariant &val) {d->value = val; return *this;}
		QVariant value() const {return d->value;}
		Option& setDefaultValue(const QVariant &val) {d->defaultValue = val; return *this;}
		QVariant defaultValue() const {return d->defaultValue;}
		Option& setComment(const QString &s) {d->comment = s; return *this;}
		QString comment() const {return d->comment;}
		Option& setMandatory(bool b) {d->mandatory = b; return *this;}
		bool isMandatory() const {return d->mandatory;}
		bool isSet() const {return value().isValid();}
	public:
		Option();
		Option(QVariant::Type type);
	};
public:
	Option& addOption(const QString key, const Option &opt = Option());
	bool setValue(const QString &name, const QVariant val, bool throw_exc = true) throw(Exception);
	Option option(const QString &name, bool throw_exc = true) const throw(Exception);
	Option& optionRef(const QString &name) throw(Exception);
	QMap<QString, Option> options() const {return m_options;}

	void parse(int argc, char *argv[]);
	virtual void parse(const QStringList &cmd_line_args);
	bool isParseError() const {return !m_parseErrors.isEmpty();}
	bool isAppBreak() const {return m_isAppBreak;}
	QStringList parseErrors() const {return m_parseErrors;}
	QStringList unusedArguments() {return m_unusedArguments;}

	void mergeConfig(const QVariantMap &config_map) {mergeConfig_helper(QString(), config_map);}

	QString applicationDir() const;
	QString applicationName() const;
	void help() const;
	void help(QTextStream &os) const;
	void dump() const;
	void dump(QTextStream &os) const;
public slots:
	QVariant value(const QString &name, const QVariant default_value = QVariant()) const;
protected:
	QPair<QString, QString> applicationDirAndName() const;
	QString takeArg();
	void addParseError(const QString &err);
	void mergeConfig_helper(const QString &key_prefix, const QVariantMap &config_map);
private:
	QMap<QString, Option> m_options;
	QStringList m_arguments;
	int m_parsedArgIndex;
	QStringList m_unusedArguments;
	QStringList m_parseErrors;
	bool m_isAppBreak;
	QStringList m_allArgs;
};

#define CLIOPTION_QUOTE_ME(x) QStringLiteral(#x)

#define CLIOPTION_GETTER_SETTER(ptype, getter_prefix, setter_prefix, name_rest) \
	public: ptype getter_prefix##name_rest() const { \
		Option opt = option(CLIOPTION_QUOTE_ME(getter_prefix##name_rest)); \
		QVariant val = opt.value(); \
		if(!val.isValid()) \
			val = opt.defaultValue(); \
		return qvariant_cast<ptype>(val); \
	} \
	public: void setter_prefix##name_rest(const ptype &val) {optionRef(CLIOPTION_QUOTE_ME(getter_prefix##name_rest)).setValue(val);}

}}}

#endif // CLIOPTIONS_H
