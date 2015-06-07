#ifndef COMPETITORS_COMPETITORSPLUGIN_H
#define COMPETITORS_COMPETITORSPLUGIN_H

#include "../competitorspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>

namespace qf {
namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
class DockWidget;
}}}

namespace Competitors {

class COMPETITORSPLUGIN_DECL_EXPORT CompetitorsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CompetitorsPlugin(QObject *parent = nullptr);
	~CompetitorsPlugin() Q_DECL_OVERRIDE;

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_INVOKABLE QObject* createCompetitorDocument(QObject *parent);

	Q_SIGNAL void nativeInstalled();
private:
	Q_SLOT void onInstalled();
	void onRegistrationsDockVisibleChanged(bool on = true);
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
	qf::qmlwidgets::framework::DockWidget *m_registrationsDockWidget = nullptr;
};

}

#endif // COMPETITORS_COMPETITORSPLUGIN_H
