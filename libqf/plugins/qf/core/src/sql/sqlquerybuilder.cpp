#include "sqlquerybuilder.h"

#include <qf/core/log.h>

using namespace qf::core::qml;

SqlQueryBuilder::SqlQueryBuilder(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this;
}

SqlQueryBuilder::~SqlQueryBuilder()
{
	qfLogFuncFrame() << this;
}
