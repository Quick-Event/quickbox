#include "tablemodelcolumn.h"

#include <qf/core/log.h>

using namespace qf::core::qml;

TableModelColumn::TableModelColumn(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame();
}

TableModelColumn::~TableModelColumn()
{
	qfLogFuncFrame();
}

