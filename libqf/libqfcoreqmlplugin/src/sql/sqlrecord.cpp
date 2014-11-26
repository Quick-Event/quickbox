#include "sqlrecord.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

using namespace qf::core::qml;

SqlRecord::SqlRecord(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this;
}

SqlRecord::~SqlRecord()
{
	qfLogFuncFrame() << this;
}

void SqlRecord::setRecord(const QSqlRecord &rec)
{
	m_record = rec;
}

QString SqlRecord::shortFieldName(int ix)
{
	QString ret;
	qf::core::Utils::parseFieldName(fieldName(ix), &ret);
	return ret;
}

QStringList SqlRecord::fieldNames()
{
	qfLogFuncFrame();
	QStringList ret;
	for(int i=0; i<count(); i++) {
		qfDebug() << i << "->" << fieldName(i);
		ret << fieldName(i);
	}
	return ret;
}
