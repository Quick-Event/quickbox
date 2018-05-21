#pragma once

#include "../relayspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>
#include <qf/core/utils/table.h>

namespace qf {

namespace core {

namespace model {
class SqlTableModel;
}}

namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
class DockWidget;
}}

}

namespace Relays {

class RELAYSPLUGIN_DECL_EXPORT RelaysPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	RelaysPlugin(QObject *parent = nullptr);
	~RelaysPlugin() Q_DECL_OVERRIDE;

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_INVOKABLE QObject* createRelayDocument(QObject *parent);
	Q_INVOKABLE int editRelay(int id, int mode);

	Q_SIGNAL void dbEventNotify(const QString &domain, int connection_id, const QVariant &payload);
	Q_SIGNAL void competitorEdited(); // used to clear caches with competitors

	Q_SIGNAL void nativeInstalled();

	Q_SLOT void reloadRegistrationsModel();
	qf::core::model::SqlTableModel* registrationsModel();
	const qf::core::utils::Table& registrationsTable();
private:
	Q_SLOT void onInstalled();
	void onRegistrationsDockVisibleChanged(bool on = true);
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
	qf::qmlwidgets::framework::DockWidget *m_registrationsDockWidget = nullptr;
	qf::core::model::SqlTableModel *m_registrationsModel = nullptr;
	qf::core::utils::Table m_registrationsTable;
};

}

