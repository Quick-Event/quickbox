#include "oris.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/query.h>
#include <qf/core/log.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

Oris::Oris(QObject *parent)
	: Super(parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("Oris");
	//connect(this, &Oris::installed, this, &Oris::onInstalled, Qt::QueuedConnection);
}

