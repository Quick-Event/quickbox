#ifndef COMPETITORSPLUGIN_H
#define COMPETITORSPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>
//#include <qf/qmlwidgets/framework/partwidget.h>

#include <qf/core/utils.h>

namespace qf {
namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
}
}
}
class CompetitorsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CompetitorsPlugin(QObject *parent = nullptr);

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_SIGNAL void nativeInstalled();
private:
	Q_SLOT void onInstalled();
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
};

#endif // COMPETITORSPLUGIN_H
