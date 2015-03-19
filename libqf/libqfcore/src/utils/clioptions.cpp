#include "clioptions.h"
#include "../core/log.h"

#include <QSettings>
#include <QStringBuilder>
#include <QStringList>
#include <QCoreApplication>

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

QVariant CLIOptions::value(const QString& name, const QVariant default_value) const
{
	QVariant ret = option(name, false).value();
	if(!ret.isValid()) ret = default_value;
	return ret;
}

void CLIOptions::setValue(const QString& name, const QVariant val, bool throw_exc) throw(Exception)
{
	Option o = option(name, false);
	if(o.isNull()) {
		QString msg = "setValue():"%val.toString()%" Key '"%name%"' not found.";
		qfWarning() << msg;
		if(throw_exc) {
			throw Exception(msg);
		}
	}
	else {
		Option &orf = optionRef(name);
		orf.setValue(val);
	}
}

QString CLIOptions::takeArg()
{
	QString ret = m_arguments.value(m_parsedArgIndex++);
	return ret;
}

void CLIOptions::parse(int argc, char* argv[])
{
	QStringList args;
	for(int i=0; i<argc; i++) args << QString::fromUtf8(argv[i]);
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
		if(arg.isEmpty()) break;
		if(arg == "--help" || arg == "-h") {
			help();
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
				if(arg.startsWith("-")) m_unusedArguments << arg;
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
}

QString CLIOptions::applicationName() const
{
	QString ret;
	if(m_allArgs.size()) {
		ret = m_allArgs[0];
#ifdef Q_OS_WIN
		QChar sep = '\\';
#else
		QChar sep = '/';
#endif
		ret = ret.section(sep, -1);
	}
	return ret;
}

void CLIOptions::help(QTextStream& os) const
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

void CLIOptions::help() const
{
	QTextStream ts(stdout);
	help(ts);
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
	//LOGDEB() << "addParseError:" << err;
	m_parseErrors << err;
	//LOGDEB() << "isParseError:" << isParseError();
}
