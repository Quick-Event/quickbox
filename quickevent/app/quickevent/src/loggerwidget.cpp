#include "loggerwidget.h"
#include "tablemodellogdevice.h"

#include <qf/core/model/logtablemodel.h>

LoggerWidget::LoggerWidget(QWidget *parent)
	: Super(parent)
{
	//m_logDevice =  LoggerLogDevice::install();
	//m_logDevice->setParent(this);
	//m_logDevice->setLogTreshold(qf::core::Log::LOG_WARN);

	addCategoryActions(tr("<empty>"), QString(), qf::core::Log::Level::Info);

	auto *ld = qobject_cast<TableModelLogDevice*>(qf::core::LogDevice::findDevice(TABLE_MODEL_LOG_DEVICE));
	setLogTableModel(ld->logTableModel());
}

LoggerWidget::~LoggerWidget()
{
	//QF_SAFE_DELETE(m_logDevice);
}

void LoggerWidget::onDockWidgetVisibleChanged(bool visible)
{
	//qfWarning() << "onDockWidgetVisibleChanged" << visible;
	if(visible) {
		auto *ld = qobject_cast<TableModelLogDevice*>(qf::core::LogDevice::findDevice(TABLE_MODEL_LOG_DEVICE));
		setLogTableModel(ld->logTableModel());
		checkScrollToLastEntry();
	}
	else {
		setLogTableModel(nullptr);
	}
}

void LoggerWidget::registerLogCategories()
{
	Super::registerLogCategories();
	auto *ld = qobject_cast<TableModelLogDevice*>(qf::core::LogDevice::findDevice(TABLE_MODEL_LOG_DEVICE));
	ld->setCategories(selectedLogCategories());
}


