#include "partwidget.h"

#include <qf/core/assert.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>


namespace quickevent {
namespace gui {

PartWidget::PartWidget(const QString& title, const QString &feature_id, QWidget *parent)
	: Super(feature_id, parent)
{
	setPersistentSettingsId(featureId());
	setTitle(title);

	connect(this, &PartWidget::activeChanged, this, &PartWidget::onActiveChanged);
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *event_plugin = fwk->plugin("Event", qf::core::Exception::Throw);
	connect(event_plugin, SIGNAL(currentStageIdChanged(int)), this, SIGNAL(resetPartRequest()));
	connect(event_plugin, SIGNAL(eventOpenChanged(bool)), this, SIGNAL(resetPartRequest()));
	connect(event_plugin, SIGNAL(reloadDataRequest()), this, SIGNAL(resetPartRequest()));
	//connect(event_plugin, SIGNAL(eventOpened()), this, SIGNAL(resetRequest()));
}

void PartWidget::onActiveChanged()
{
	if(isActive()) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		QF_ASSERT(fwk != nullptr, "Invalid FrameWork", return);
		qf::qmlwidgets::framework::Plugin *event_plugin = fwk->plugin("Event", qf::core::Exception::Throw);
		bool sql_connected = event_plugin->property("sqlServerConnected").toBool();
		QString event_name = event_plugin->property("eventName").toString();
		if(sql_connected && !event_name.isEmpty()) {
			emit reloadPartRequest();
		}
	}
}

}}
