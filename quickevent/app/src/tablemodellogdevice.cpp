#include "tablemodellogdevice.h"

#include <qf/core/model/logtablemodel.h>

TableModelLogDevice::TableModelLogDevice(QObject *parent)
 : Super(parent)
{
	m_logFilter.categoriesTresholds[QString()] = qf::core::Log::Level::Info;
	m_logTableModel = new qf::core::model::LogTableModel(this);
	QObject::connect(this, &qf::core::SignalLogDevice::logEntry, m_logTableModel, &qf::core::model::LogTableModel::addLogEntry);
}

qf::core::model::LogTableModel *TableModelLogDevice::logTableModel()
{
	return m_logTableModel;
}

void TableModelLogDevice::setCategories(const QMap<QString, qf::core::Log::Level> &cats)
{
	m_logFilter.categoriesTresholds = cats;
}

bool TableModelLogDevice::isMatchingLogFilter(qf::core::Log::Level level, const char *file_name, const char *category)
{
	Q_UNUSED(file_name)
	QString cat = category;
	qf::core::Log::Level treshold_level = m_logFilter.categoriesTresholds.value(cat, qf::core::Log::Level::Invalid);
	bool ok = (level <= treshold_level);
	//if(category && category[0])
	//	printf("%p %s %s:%d vs. %d -> %d\n", this, file_name, category, level, treshold_level, ok);
	return ok;
}

