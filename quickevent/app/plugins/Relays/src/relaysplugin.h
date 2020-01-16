#pragma once

#include "relayspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>

namespace qf {

namespace core {  namespace model { class SqlTableModel; }}
namespace core {  namespace utils { class TreeTable; }}

namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
class DockWidget;
}}

}

namespace quickevent {  namespace core {  namespace si { class CheckedCard; }}}

namespace Relays {

class RELAYSPLUGIN_DECL_EXPORT RelaysPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
private:
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	RelaysPlugin(QObject *parent = nullptr);
	~RelaysPlugin() Q_DECL_OVERRIDE;

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_INVOKABLE QObject* createRelayDocument(QObject *parent);
	Q_INVOKABLE int editRelay(int id, int mode);

	Q_SIGNAL void dbEventNotify(const QString &domain, int connection_id, const QVariant &payload);
	Q_SIGNAL void competitorEdited(); // used to clear caches with competitors

	Q_SIGNAL void nativeInstalled();

	qf::core::utils::TreeTable nLegsResultsTable(const QString &where_option, int leg_count, int places, bool exclude_not_finish);
	qf::core::utils::TreeTable nLegsClassResultsTable(int class_id, int leg_count, int places, bool exclude_not_finish);
private:
	Q_SLOT void onInstalled();
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void processRunnerFinished(const quickevent::core::si::CheckedCard &checked_card);
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
};

}

