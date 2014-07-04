#include "centralwidget.h"
#include "partwidget.h"
#include "mainwindow.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::framework;

CentralWidget::CentralWidget(MainWindow *parent) :
	Super(parent), m_frameWork(parent)
{
	Q_ASSERT(m_frameWork != nullptr);
}


