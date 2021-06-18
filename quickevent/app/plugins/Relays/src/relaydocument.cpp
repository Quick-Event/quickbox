#include "relaydocument.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
//#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>

using namespace Relays;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;

 RelayDocument:: RelayDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("relays", "*")
			.select2("classes", "name")
			.from("relays")
			.join("relays.classId", "classes.id")
			.where("relays.id={{ID}}");
	setQueryBuilder(qb);
}

bool  RelayDocument::saveData()
{
	qfLogFuncFrame();
	bool ret = Super::saveData();
	return ret;
}

bool  RelayDocument::dropData()
{
	bool ret = false;
	auto id = dataId();
	{
		qf::core::sql::Query q(model()->connectionName());
		q.prepare("UPDATE runs SET relayId=NULL WHERE relayId = :relayId");
		q.bindValue(":relayId", id);
		ret = q.exec();
		if(!ret)
			qfError() << q.lastError().text();
	}
	if(ret) {
		ret = Super::dropData();
		getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
	}
	return ret;
}

