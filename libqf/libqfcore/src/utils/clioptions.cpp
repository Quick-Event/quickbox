#include "clioptions.h"
#include "../core/log.h"
#include "../core/assert.h"
#include "../core/utils.h"

#include <QStringBuilder>
#include <QStringList>
#include <QDir>
#include <QJsonParseError>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include <limits>

using namespace qf::core::utils;

const CLIOptions::Option & CLIOptions::Option::sharedNull()
{
	static Option n = Option(NullConstructor());
	return n;
}

CLIOptions::Option::Option(CLIOptions::Option::NullConstructor)
{
	d = new Data();
}

CLIOptions::Option::Option()
{
	*this = sharedNull();
}

CLIOptions::Option::Option(QVariant::Type type)
{
	d = new Data(type);
}

CLIOptions::Option& CLIOptions::Option::setValueString(const QString& val)
{
	QVariant::Type t = type();
	switch(t) {
	case(QVariant::Invalid):
		qfWarning() << "Setting value:" << val << "to an invalid type option.";
		break;
	case(QVariant::Int):
	{
		bool ok;
		setValue(val.toInt(&ok));
		if(!ok)
			qfWarning() << "Value:" << val << "cannot be converted to Int.";
		break;
	}
	case(QVariant::Double):
	{
		bool ok;
		setValue(val.toDouble(&ok));
		if(!ok) qfWarning() << "Value:" << val << "cannot be converted to Double.";
		break;
	}
	default:
		setValue(val);
	}
	return *this;
}

CLIOptions::CLIOptions(QObject *parent)
	: QObject(parent)
{
	addOption("abortOnException").setType(QVariant::Bool).setNames("--abort-on-exception").setComment(tr("Abort application on exception"));
	addOption("help").setType(QVariant::Bool).setNames("-h", "--help").setComment(tr("Print help"));
	addOption("config").setType(QVariant::String).setNames("--config").setComment(tr("Config name, it is loaded from {app-name}[.conf] if file exists in {config-path}"));
	addOption("configDir").setType(QVariant::String).setNames("--config-dir").setComment("Directory where server config fiels are searched, default value: {app-dir-path}.");
}

CLIOptions::~CLIOptions()
{
}

CLIOptions::Option& CLIOptions::addOption(const QString key, const CLIOptions::Option& opt)
{
	m_options[key] = opt;
	return m_options[key];
}

CLIOptions::Option CLIOptions::option(const QString& name, bool throw_exc) const throw(Exception)
{
	Option ret = m_options.value(name);
	if(ret.isNull() && throw_exc) {
		QString msg = "Key '"%name%"' not found.";
		qfWarning() << msg;
		throw Exception(msg);
	}
	return ret;
}

CLIOptions::Option& CLIOptions::optionRef(const QString& name) throw(Exception)
{
	if(!m_options.contains(name)) {
		QString msg = "Key '"%name%"' not found.";
		qfWarning() << msg;
		throw Exception(msg);
	}
	return m_options[name];
}

QVariantMap CLIOptions::values() const
{
	QVariantMap ret;
	QMapIterator<QString, Option> it(m_options);
	while(it.hasNext()) {
		it.next();
		ret[it.key()] = value(it.key());
	}
	return ret;
}

QVariant CLIOptions::value(const QString &name) const
{
	Option opt = option(name, qf::core::Exception::Throw);
	QVariant ret = opt.value();
	if(!ret.isValid())
		ret = opt.defaultValue();
	if(!ret.isValid())
		ret = QVariant(opt.type());
	return ret;
}

QVariant CLIOptions::value(const QString& name, const QVariant default_value) const
{
	QVariant ret = value(name);
	if(!ret.isValid())
		ret = default_value;
	return ret;
}

bool CLIOptions::setValue(const QString& name, const QVariant val, bool throw_exc) throw(Exception)
{
	Option o = option(name, false);
	if(o.isNull()) {
		QString msg = "setValue():"%val.toString()%" Key '"%name%"' not found.";
		qfWarning() << msg;
		if(throw_exc) {
			throw Exception(msg);
		}
		return false;
	}
	else {
		Option &orf = optionRef(name);
		orf.setValue(val);
	}
	return true;
}

QString CLIOptions::takeArg()
{
	QString ret = m_arguments.value(m_parsedArgIndex++);
	return ret;
}

void CLIOptions::parse(int argc, char* argv[])
{
	QStringList args;
	for(int i=0; i<argc; i++)
		args << QString::fromUtf8(argv[i]);
	parse(args);
}

void CLIOptions::parse(const QStringList& cmd_line_args)
{
	qfLogFuncFrame() << cmd_line_args.join(' ');
	m_isAppBreak = false;
	m_parsedArgIndex = 0;
	m_arguments = cmd_line_args.mid(1);
	m_unusedArguments = QStringList();
	m_parseErrors = QStringList();
	m_allArgs = cmd_line_args;

	while(true) {
		QString arg = takeArg();
		if(arg.isEmpty())
			break;
		if(arg == "--help" || arg == "-h") {
			setHelp(true);
			printHelp();
			m_isAppBreak = true;
			return;
		}
		else {
			bool found = false;
			QMutableMapIterator<QString, Option> it(m_options);
			while(it.hasNext()) {
				it.next();
				Option &opt = it.value();
				QStringList names = opt.names();
				if(names.contains(arg)) {
					found = true;
					if(opt.type() != QVariant::Bool) {
						arg = takeArg();
						opt.setValueString(arg);
					}
					else {
						//LOGDEB() << "setting true";
						opt.setValue(true);
					}
					break;
				}
			}
			if(!found) {
				if(arg.startsWith("-"))
					m_unusedArguments << arg;
			}
		}
	}
	{
		QMapIterator<QString, Option> it(m_options);
		while(it.hasNext()) {
			it.next();
			Option opt = it.value();
			//LOGDEB() << "option:" << it.key() << "is mandatory:" << opt.isMandatory() << "is valid:" << opt.value().isValid();
			if(opt.isMandatory() && !opt.value().isValid()) {
				addParseError(QString("Mandatory option '%1' not set.").arg(opt.names().value(0)));
			}
		}
	}
	qf::core::Exception::setAbortOnException(isAbortOnException());
}

QPair<QString, QString> CLIOptions::applicationDirAndName() const
{
	static QString app_dir;
	static QString app_name;
	if(app_name.isEmpty()) {
		if(m_allArgs.size()) {
	#ifdef Q_OS_WIN
			//static constexpr int MAX_PATH = 1024;
			QString app_file_path;
			wchar_t buffer[MAX_PATH + 2];
			DWORD v = GetModuleFileName(0, buffer, MAX_PATH + 1);
			buffer[MAX_PATH + 1] = 0;
			if (v <= MAX_PATH)
				app_file_path = QString::fromWCharArray(buffer);
			QChar sep = '\\';
	#else
			QString app_file_path = m_allArgs[0];
			QChar sep = '/';
	#endif
			app_dir = app_file_path.section(sep, 0, -2);
			app_name = app_file_path.section(sep, -1);
			qfInfo() << "app dir:" << app_dir << "name:" << app_name;
	#ifdef Q_OS_WIN
			if(app_name.endsWith(QLatin1String(".exe"), Qt::CaseInsensitive))
				app_name = app_name.mid(0, app_name.length() - 4);
	#endif
		}
	}
	return QPair<QString, QString>(app_dir, app_name);
}

QString CLIOptions::applicationDir() const
{
	return QDir::fromNativeSeparators(applicationDirAndName().first);
}

QString CLIOptions::applicationName() const
{
	return applicationDirAndName().second;
}

void CLIOptions::printHelp(QTextStream& os) const
{
	os << applicationName() << " [OPTIONS]" << endl << endl;
	os << "OPTIONS:" << endl << endl;
	QMapIterator<QString, Option> it(m_options);
	while(it.hasNext()) {
		it.next();
		Option opt = it.value();
		os << opt.names().join(", ");
		if(opt.type() != QVariant::Bool) {
			if(opt.type() == QVariant::Int || opt.type() == QVariant::Double) os << " " << "number";
			else os << " " << "'string'";
		}
		os << ':';
		QVariant def_val = opt.defaultValue();
		if(def_val.isValid()) os << " [default(" << def_val.toString() << ")]";
		if(opt.isMandatory()) os << " [MANDATORY]";
		os << endl;
		QString oc = opt.comment();
		if(!oc.isEmpty()) os << "\t" << opt.comment() << endl;
	}
}

void CLIOptions::printHelp() const
{
	QTextStream ts(stdout);
	printHelp(ts);
}

void CLIOptions::dump(QTextStream &os) const
{
	QMapIterator<QString, Option> it(m_options);
	while(it.hasNext()) {
		it.next();
		Option opt = it.value();
		os << it.key() << '(' << opt.names().join(", ") << ')' << ": " << opt.value().toString() << endl;
	}
}

void CLIOptions::dump() const
{
	QTextStream ts(stdout);
	ts << "=============== options values dump ==============" << endl;
	dump(ts);
	ts << "-------------------------------------------------" << endl;
}

void CLIOptions::addParseError(const QString& err)
{
	m_parseErrors << err;
}

ConfigCLIOptions::ConfigCLIOptions(QObject *parent)
	: Super(parent)
{
	addOption("config").setType(QVariant::String).setNames("--config").setComment("Application config name, it is loaded from {config}[.conf] if file exists in {config-path}, deault value is {app-name}.conf");
	addOption("configDir").setType(QVariant::String).setNames("--config-dir").setComment("Directory where application config fiels are searched, default value: {app-dir-path}.");
}

void ConfigCLIOptions::parse(const QStringList &cmd_line_args)
{
	Super::parse(cmd_line_args);
	if(config().isEmpty())
		setConfig(applicationName());
}

bool ConfigCLIOptions::loadConfigFile()
{
	QString config_dir = configDir();
	if(config_dir.isEmpty())
		config_dir = applicationDir();
	QString config_file = config();
	qfInfo() << "config-dir:" << config_dir << "config-file:" << config_file;
	if(!config_file.isEmpty()) {
		if(!config_file.contains('.'))
			config_file += ".conf";
		config_file = config_dir + '/' + config_file;
		QFile f(config_file);
		qfInfo() << "Checking presence of config file:" << f.fileName();
		if(f.open(QFile::ReadOnly)) {
			qfInfo() << "Reading config file:" << f.fileName();
			QString str = QString::fromUtf8(f.readAll());
			str = qf::core::Utils::removeJsonComments(str);
			qfDebug() << str;
			QJsonParseError err;
			auto jsd = QJsonDocument::fromJson(str.toUtf8(), &err);
			if(err.error == QJsonParseError::NoError) {
				mergeConfig(jsd.toVariant().toMap());
			}
			else {
				qfError() << "Error parsing config file:" << f.fileName() << "on offset:" << err.offset << err.errorString();
				return false;
			}
		}
	}
	return true;
}

void ConfigCLIOptions::mergeConfig_helper(const QString &key_prefix, const QVariantMap &config_map)
{
	//qfLogFuncFrame() << key_prefix;
	QMapIterator<QString, QVariant> it(config_map);
	while(it.hasNext()) {
		it.next();
		QString key = it.key().trimmed();
		QF_ASSERT(!key.isEmpty(), "Empty key!", continue);
		if(!key_prefix.isEmpty()) {
			key = key_prefix + '.' + key;
		}
		QVariant v = it.value();
		if(v.type() == QVariant::Map) {
			QVariantMap m = v.toMap();
			mergeConfig_helper(key, m);
		}
		else {
			try {
				Option &opt = optionRef(key);
				if(!opt.isSet()) {
					//qfInfo() << key << "-->" << v;
					opt.setValue(v);
				}
			}
			catch(const qf::core::Exception &e) {
				qfWarning() << "Merge option" << key << "error:" << e.message();
			}
		}
	}
}

