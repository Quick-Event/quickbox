#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("profile").setType(QVariant::String).setNames("--profile").setComment("Profile name [startlist | results]").setDefaultValue("results");
	addOption("application.refreshTime").setType(QVariant::String).setNames("--refresh-time").setDefaultValue(1000).setComment("refresh time in msec");
	addOption("application.columnCount").setType(QVariant::Int).setNames("-c", "--column-count").setDefaultValue(2);
	addOption("application.fontScale").setType(QVariant::Int).setNames("--font-scale").setDefaultValue(100).setComment("font scale in %");
	addOption("event.name").setType(QVariant::String).setNames("-e", "--event");
	addOption("event.stage").setType(QVariant::Int).setNames("-n", "--stage").setComment("If not set, the current stage number is loaded from database.");
	addOption("connection.host").setType(QVariant::String).setNames("-s", "--host").setDefaultValue("localhost");
	addOption("connection.port").setType(QVariant::Int).setNames("--port").setDefaultValue(5432);
	addOption("connection.user").setType(QVariant::String).setNames("-u", "--user").setDefaultValue("quickevent");
	addOption("connection.password").setType(QVariant::String).setNames("-p", "--password");
	addOption("connection.database").setType(QVariant::String).setNames("-b", "--database").setDefaultValue("quickevent");
	addOption("connection.driver").setType(QVariant::String).setNames("--sql-driver").setDefaultValue("QPSQL");
}
