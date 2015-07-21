#ifndef RUNS_RUNSPLUGIN_H
#define RUNS_RUNSPLUGIN_H

#include "../runspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>

namespace qf {
	namespace core {
		namespace utils {
			class Table;
		}
	}
	namespace qmlwidgets {
		class Action;
		namespace framework {
			class PartWidget;
			class DockWidget;
		}
	}
}

namespace Runs {

class RUNSPLUGIN_DECL_EXPORT RunsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
	Q_PROPERTY(int selectedStageId READ selectedStageId WRITE setSelectedStageId NOTIFY selectedStageIdChanged)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	static constexpr int UNREAL_TIME_MS = 999 * 60 * 1000;
public:
	RunsPlugin(QObject *parent = nullptr);
	~RunsPlugin() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL2(int, s, S, electedStageId, 1)

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_SIGNAL void nativeInstalled();

	Q_INVOKABLE int courseForRun(int run_id);
	qf::core::utils::Table nstagesResultsTable(int stages_count, int class_id, int places = -1);
	Q_INVOKABLE QVariant nstagesResultsTableData(int stages_count, int places = -1);

private:
	Q_SLOT void onInstalled();
	Q_SLOT void onEditStartListRequest(int stage_id, int class_id, int competitor_id);

	int courseForRun_Classic(int run_id);
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
};

}

#endif // RUNS_RUNSPLUGIN_H
