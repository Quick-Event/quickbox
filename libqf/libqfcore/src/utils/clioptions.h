#ifndef QF_CORE_UTILS_CLIOPTIONS_H
#define QF_CORE_UTILS_CLIOPTIONS_H

#include "../core/coreglobal.h"
#include "../core/exception.h"

#include "../core/exception.h"

#include <QObject>
#include <QVariantMap>
#include <QSharedData>
#include <QStringList>
/*
#if defined(_MSC_VER)
// disable throw() warning for MS VC
#pragma warning( disable : 4290 )
#endif
*/

class QTextStream;

namespace qf {
namespace core {
namespace utils {

#define CLIOPTION_QUOTE_ME(x) QStringLiteral(#x)

#define CLIOPTION_GETTER_SETTER(ptype, getter_prefix, setter_prefix, name_rest) \
	public: ptype getter_prefix##name_rest() const { \
		QVariant val = value(CLIOPTION_QUOTE_ME(getter_prefix##name_rest)); \
		return qvariant_cast<ptype>(val); \
	} \
	public: bool getter_prefix##name_rest##_isset() const {return isValueSet(CLIOPTION_QUOTE_ME(getter_prefix##name_rest));} \
	public: bool setter_prefix##name_rest(const ptype &val) {return setValue(CLIOPTION_QUOTE_ME(getter_prefix##name_rest), val);}

#define CLIOPTION_GETTER_SETTER2(ptype, pkey, getter_prefix, setter_prefix, name_rest) \
	public: ptype getter_prefix##name_rest() const { \
		QVariant val = value(QStringLiteral(pkey)); \
		return qvariant_cast<ptype>(val); \
	} \
	public: bool getter_prefix##name_rest##_isset() const {return isValueSet(QStringLiteral(pkey));} \
	public: bool setter_prefix##name_rest(const ptype &val) {return setValue(QStringLiteral(pkey), val);}

class QFCORE_DECL_EXPORT CLIOptions : public QObject
{
	Q_OBJECT
public:
	CLIOptions(QObject *parent = NULL);
	virtual ~CLIOptions();

	CLIOPTION_GETTER_SETTER2(bool, "abortOnException", is, set, AbortOnException)
	CLIOPTION_GETTER_SETTER2(bool, "help", is, set, Help)
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
		Option& setValueString(const QString &val_str);
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
	Option option(const QString &name, bool throw_exc = true) const throw(Exception);
	Option& optionRef(const QString &name) throw(Exception);
	QMap<QString, Option> options() const {return m_options;}

	void parse(int argc, char *argv[]);
	virtual void parse(const QStringList &cmd_line_args);
	bool isParseError() const {return !m_parseErrors.isEmpty();}
	bool isAppBreak() const {return m_isAppBreak;}
	QStringList parseErrors() const {return m_parseErrors;}
	QStringList unusedArguments() {return m_unusedArguments;}

	Q_INVOKABLE QString applicationDir() const;
	Q_INVOKABLE QString applicationName() const;
	Q_INVOKABLE void printHelp() const;
	void printHelp(QTextStream &os) const;
	Q_INVOKABLE void dump() const;
	void dump(QTextStream &os) const;

	Q_INVOKABLE bool optionExists(const QString &name) const;
	Q_INVOKABLE QVariantMap values() const;
	Q_INVOKABLE QVariant value(const QString &name) const;
	Q_INVOKABLE QVariant value(const QString &name, const QVariant default_value) const;
	/// value is explicitly set from command line or in config file
	/// defaultValue is not considered to be an explicitly set value
	Q_INVOKABLE bool isValueSet(const QString &name) const;
	bool setValue(const QString &name, const QVariant val, bool throw_exc = true) throw(Exception);
protected:
	QVariant value_helper(const QString &name, bool throw_exception) const;
	QPair<QString, QString> applicationDirAndName() const;
	QString takeArg();
	QString peekArg() const;
	void addParseError(const QString &err);
private:
	QMap<QString, Option> m_options;
	QStringList m_arguments;
	int m_parsedArgIndex;
	QStringList m_unusedArguments;
	QStringList m_parseErrors;
	bool m_isAppBreak;
	QStringList m_allArgs;
};

class QFCORE_DECL_EXPORT ConfigCLIOptions : public CLIOptions
{
	Q_OBJECT
private:
	typedef CLIOptions Super;
public:
	ConfigCLIOptions(QObject *parent = NULL);
	~ConfigCLIOptions() Q_DECL_OVERRIDE {}

	CLIOPTION_GETTER_SETTER(QString, c, setC, onfig)
	CLIOPTION_GETTER_SETTER(QString, c, setC, onfigDir)

	void parse(const QStringList &cmd_line_args) Q_DECL_OVERRIDE;
	bool loadConfigFile();
protected:
	QString configFile();
protected:
	void mergeConfig(const QVariantMap &config_map) {mergeConfig_helper(QString(), config_map);}
	void mergeConfig_helper(const QString &key_prefix, const QVariantMap &config_map);
};

}}}

#endif
