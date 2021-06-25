#include "speakerplugin.h"
#include "speakerwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/query.h>
#include <qf/core/log.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;
using quickevent::gui::PartWidget;

namespace Speaker {

SpeakerPlugin::SpeakerPlugin(QObject *parent)
	: Super("Speaker", parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("SpeakerPlugin");
	connect(this, &SpeakerPlugin::installed, this, &SpeakerPlugin::onInstalled, Qt::QueuedConnection);
}

void SpeakerPlugin::onInstalled()
{
	qfLogFuncFrame();
	qff::initPluginWidget<SpeakerWidget, PartWidget>(tr("Speaker"), featureId());
}

}
