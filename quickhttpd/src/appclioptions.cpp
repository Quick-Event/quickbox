#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("application.htmlDir").setType(QVariant::String).setNames("--html-dir").setDefaultValue("./html").setComment("directory where HTML pages will be stored");
	addOption("application.refreshTime").setType(QVariant::Int).setNames("--refresh-time").setDefaultValue(60*1000).setComment("refresh time in msec");
	addOption("event.name").setType(QVariant::String).setNames("-e", "--event");
	addOption("event.stage").setType(QVariant::Int).setNames("-n", "--stage").setComment("If not set, the current stage number is loaded from database.");
	addOption("event.classesLike").setType(QVariant::String).setNames("--classes-like").setComment("SQL LIKE expression to filter classes to show, for ex. --classes-like \"H%\"");
	addOption("event.classesNotLike").setType(QVariant::String).setNames("--classes-not-like").setComment("SQL LIKE expression to filter classes not to show, for ex. --not-classes-like \"HDR\"");
	addOption("sql.host").setType(QVariant::String).setNames("-s", "--sql-host").setDefaultValue("localhost");
	addOption("sql.port").setType(QVariant::Int).setNames("--sql-port").setDefaultValue(5432);
	addOption("sql.user").setType(QVariant::String).setNames("-u", "--sql-user").setDefaultValue("quickevent");
	addOption("sql.password").setType(QVariant::String).setNames("-p", "--sql-password");
	addOption("sql.database").setType(QVariant::String).setNames("-b", "--sql-database").setDefaultValue("quickevent");
	addOption("sql.driver").setType(QVariant::String).setNames("--sql-driver").setDefaultValue("QPSQL");

	addOption("http.port").setType(QVariant::Int).setNames("--http-port").setDefaultValue(8888);
}
