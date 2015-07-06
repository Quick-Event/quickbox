#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: CLIOptions(parent)
{
	addOption("config").setType(QVariant::String).setNames("--config").setComment("Config name, it is loaded from {config}[.conf] if file exists in {config-path}").setDefaultValue("quickshow");
	addOption("configDir").setType(QVariant::String).setNames("--config-dir").setComment("Directory where server config fiels are searched, default value: {app-dir-path}.");
	addOption("profile").setType(QVariant::String).setNames("--profile").setComment("Profile name").setDefaultValue("results");
	addOption("application.refreshTime").setType(QVariant::String).setNames("--refresh-time").setDefaultValue(1000).setComment("refresh time in msec");
	addOption("event.name").setType(QVariant::String).setNames("-e", "--event");
	addOption("event.stage").setType(QVariant::Int).setNames("-s", "--stage").setDefaultValue(1);
	addOption("connection.host").setType(QVariant::String).setNames("-h", "--host");
	addOption("connection.port").setType(QVariant::Int).setNames("--port").setDefaultValue(5432);
	addOption("connection.user").setType(QVariant::String).setNames("-u", "--user");
	addOption("connection.password").setType(QVariant::String).setNames("-p", "--password");
	addOption("connection.database").setType(QVariant::String).setNames("-d", "--database").setDefaultValue("quickevent");
}
