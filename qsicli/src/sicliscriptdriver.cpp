
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "sicliscriptdriver.h"

#include <QScriptEngine>
#include <QSettings>

#include <qflogcust.h>
#include "theapp.h"

//=================================================
//             SICliScriptDriver
//=================================================
SICliScriptDriver::SICliScriptDriver(QObject *parent)
: QFSqlScriptDriverBase(parent)
{
}

SICliScriptDriver::~SICliScriptDriver()
{
}

QScriptEngine* SICliScriptDriver::scriptEngine()
{
	QScriptEngine *ret = QFScriptDriver::scriptEngine();
	return ret;
}

QScriptValue SICliScriptDriver::callExtensionFunction(const QString& fn, const QVariantList& params) throw(QFException)
{
	qfLogFuncFrame();
	QScriptValue ret;
	QSettings settings;
	QString extension = settings.value("app/extensions/currentName").toString();
	QString ext_property_name = extension;
	ext_property_name.replace('.', '_');
	ext_property_name = "__sicli_extension_" + ext_property_name;
	if(extension.isEmpty()) return ret;
	QScriptValue mo = scriptEngine()->globalObject().property(ext_property_name);
	qfTrash() << "\t extension module object is undefined:" << mo.isUndefined() << "is valid:" << mo.isValid() << "is null:" << mo.isNull() << "is object:" << mo.isObject();
	if(!mo.isValid()) {
		qfTrash() << "\t loading extension module object";
		mo = constructedModuleObject(extension, QScriptValueList(), Qf::ThrowExc);
		/*
		if(!mo.isObject()) {
			QF_EXCEPTION(tr("Cann't load extension object '%1'").arg(extension));
			//return ret;
		}
		*/
		scriptEngine()->globalObject().setProperty(ext_property_name, mo);
		qfTrash() << "\t loaded extension module object is undefined:" << mo.isUndefined() << "is valid:" << mo.isValid() << "is null:" << mo.isNull() << "is object:" << mo.isObject();
	}
	{
		QScriptValueList args;
		foreach(QVariant v, params) args << variantToScriptValue(v);

		QScriptValue jsfn = mo.property(fn);
		if(jsfn.isFunction()) {
			//qfTrash() << "\t jsfn:" << jsfn.toString();
			QScriptValue sv = callFunction(jsfn, mo, args, !Qf::ThrowExc);
			//qfInfo() << scriptValueToJsonString(sv);
			if(sv.isError()) {
				/// zprava ze skriptu, neco se nepodarilo
				//theApp()->emitLogRequest(QFLog::LOG_ERR, sv.property("message").toString() + "\n" + sv.property("debugInfo").toString());
				theApp()->emitLogRequestPre(QFLog::LOG_ERR, sv.property("message").toString());
				//qfError() << sv.property("message").toString();
			}
			else ret = sv;
		}
		else {
			theApp()->emitLogRequest(QFLog::LOG_ERR, tr("Extension '%1' does not contains function '%2'").arg(extension).arg(fn));
			//qfError() << "Extension" << extension << "does not contains function" << fn;
		}
	}
	return ret;
}

void SICliScriptDriver::appendCardLogLine(const QString &line)
{
	theApp()->cardLog() << line << endl;
}
