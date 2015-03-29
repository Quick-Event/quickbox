#include "partwidget.h"

#include <qf/core/assert.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

using namespace quickevent;

PartWidget::PartWidget(const QString &feature_id, QWidget *parent)
	: Super(feature_id, parent)
{
	connect(this, &PartWidget::activeChanged, this, &PartWidget::onActiveChanged);
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *event_plugin = fwk->plugin("Event", qf::core::Exception::Throw);
	connect(event_plugin, SIGNAL(currentStageChanged(int)), this, SIGNAL(resetPartRequest()));
	connect(event_plugin, SIGNAL(eventOpened(QString)), this, SIGNAL(resetPartRequest()));
	connect(event_plugin, SIGNAL(reloadDataRequest()), this, SLOT(onActiveChanged()));
	//connect(event_plugin, SIGNAL(eventOpened()), this, SIGNAL(resetRequest()));
}

void PartWidget::onActiveChanged()
{
	if(isActive()) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		QF_ASSERT(fwk != nullptr, "Invalid FrameWork", return);
		qf::qmlwidgets::framework::Plugin *event_plugin = fwk->plugin("Event", qf::core::Exception::Throw);
		bool db_open = event_plugin->property("dbOpen").toBool();
		QString event_name = event_plugin->property("eventName").toString();
		if(db_open && !event_name.isEmpty()) {
			emit reloadPartRequest();
		}
	}
}

