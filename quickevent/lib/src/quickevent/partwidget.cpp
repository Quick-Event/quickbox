#include "partwidget.h"

#include <qf/core/assert.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

using namespace quickevent;

PartWidget::PartWidget(QWidget *parent)
	: Super(parent)
{
	connect(this, &PartWidget::activeChanged, this, &PartWidget::onActiveChanged);
}

void PartWidget::onActiveChanged(bool is_active)
{
	if(is_active) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		QF_ASSERT(fwk != nullptr, "Invalid FrameWork", return);
		qf::qmlwidgets::framework::Plugin *event_plugin = fwk->plugin("Event", qf::core::Exception::Throw);
		bool db_open = event_plugin->property("dbOpen").toBool();
		QString event_name = event_plugin->property("eventName").toString();
		if(db_open && !event_name.isEmpty()) {
			emit reloadRequest();
		}
	}
}

