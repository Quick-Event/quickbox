
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SICLISCRIPTDRIVER_H
#define SICLISCRIPTDRIVER_H

#include <qfscriptdriver.h>

class QScriptEngine;

//! TODO: write class documentation.
class  SICliScriptDriver : public QFSqlScriptDriverBase
{
	Q_OBJECT;
	public:
		virtual QScriptEngine *scriptEngine();
		QScriptValue callExtensionFunction(const QString &fn, const QVariantList &params = QVariantList()) throw(QFException);
	public slots:
		void appendCardLogLine(const QString &line);
	public:
		SICliScriptDriver(QObject *parent = NULL);
		virtual ~SICliScriptDriver();
};

#endif // SICLISCRIPTDRIVER_H

