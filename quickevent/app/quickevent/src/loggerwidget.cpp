#include "loggerwidget.h"
#include "application.h"

#include <qf/qmlwidgets/log.h>

#include <qf/core/model/logtablemodel.h>

LoggerWidget::LoggerWidget(QWidget *parent)
	: Super(parent)
{
	addCategoryActions(tr("<empty>"), QString(), NecroLog::Level::Info);

	m_logModel = new qf::core::model::LogTableModel(this);
	connect(Application::instance(), &Application::newLogEntry, m_logModel, &qf::core::model::LogTableModel::addLogEntry, Qt::QueuedConnection);
	setLogTableModel(m_logModel);
}

LoggerWidget::~LoggerWidget()
{
	//QF_SAFE_DELETE(m_logDevice);
}

void LoggerWidget::onDockWidgetVisibleChanged(bool visible)
{
	//qfWarning() << "onDockWidgetVisibleChanged" << visible;
	if(visible) {
		checkScrollToLastEntry();
	}
}

void LoggerWidget::registerLogCategories()
{
	Super::registerLogCategories();
	//qfError() << "LoggerWidget::registerLogCategories" << "IMPL missing";
	//auto *ld = qobject_cast<TableModelLogDevice*>(qf::core::LogDevice::findDevice(TABLE_MODEL_LOG_DEVICE));
	//ld->setCategories(selectedLogCategories());
}


