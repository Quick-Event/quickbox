#ifndef SPEAKER_H
#define SPEAKER_H

#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/ipersistentsettings.h>

namespace qf {

namespace core {  namespace model { class SqlTableModel; }}

namespace qmlwidgets {
class Action;
namespace framework { class PartWidget; class DockWidget; }
}

}

namespace Speaker {

class SpeakerPlugin : public qf::qmlwidgets::framework::Plugin, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	SpeakerPlugin(QObject *parent = nullptr);
private:
	void onInstalled();
// 	void doDbVacuum();
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
};

}

#endif
