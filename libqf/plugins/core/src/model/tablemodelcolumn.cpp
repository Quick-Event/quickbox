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

void TableModelColumn::setFieldName(QString arg)
{
	m_columnDefinition.setFieldName(arg);
	if(m_columnIndex >= 0)
		emit updated(m_columnIndex);
}

void TableModelColumn::setCaption(QString arg)
{
	m_columnDefinition.setCaption(arg);
	if(m_columnIndex >= 0)
		emit updated(m_columnIndex);
}

void TableModelColumn::setReadOnly(bool arg)
{
	m_columnDefinition.setReadOnly(arg);
	if(m_columnIndex >= 0)
		emit updated(m_columnIndex);
}

void TableModelColumn::setCastType(int arg)
{
	//qf::core::model::TableModel *m = qobject_cast<qf::core::model::TableModel*>(parent());
	//qfWarning() << arg << m;
	m_columnDefinition.setCastType(arg);
	if(m_columnIndex >= 0)
		emit updated(m_columnIndex);
}

