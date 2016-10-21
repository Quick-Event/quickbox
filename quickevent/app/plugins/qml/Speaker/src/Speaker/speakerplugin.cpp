#include "speakerplugin.h"
#include "../thispartwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/query.h>
#include <qf/core/log.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

namespace Speaker {

SpeakerPlugin::SpeakerPlugin(QObject *parent)
	: Super(parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("SpeakerPlugin");
	connect(this, &SpeakerPlugin::installed, this, &SpeakerPlugin::onInstalled, Qt::QueuedConnection);
}

void SpeakerPlugin::onInstalled()
{
	qfLogFuncFrame();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());
}

}
