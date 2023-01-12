#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("locale").setType(QMetaType::QString).setNames("--locale").setComment("Application locale (e.g. cs_CZ for Czech, en_US for English etc.)").setDefaultValue("system");
	addOption("profile").setType(QMetaType::QString).setNames("--profile").setComment("Profile name [startlist | results]").setDefaultValue("results");
	addOption("application.refreshTime").setType(QMetaType::Int).setNames("--refresh-time").setDefaultValue(1000).setComment("refresh time in msec");
	addOption("application.columnCount").setType(QMetaType::Int).setNames("-c", "--column-count").setDefaultValue(2);
	addOption("application.fontScale").setType(QMetaType::Int).setNames("--font-scale").setDefaultValue(100).setComment("font scale in %");
	addOption("event.name").setType(QMetaType::QString).setNames("-e", "--event");
	addOption("event.stage").setType(QMetaType::Int).setNames("-n", "--stage").setComment("If not set, the current stage number is loaded from database.");
	addOption("event.classesLike").setType(QMetaType::QString).setNames("--classes-like").setComment("SQL LIKE expression to filter classes to show, for ex. --classes-like \"H%\"");
	addOption("event.classesNotLike").setType(QMetaType::QString).setNames("--classes-not-like").setComment("SQL LIKE expression to filter classes not to show, for ex. --not-classes-like \"HDR\"");
	addOption("event.classesIn").setType(QMetaType::QString).setNames("--classes-in").setComment("list to filter classes to show, for ex. --classes-in \"(\'HDR\', \'H21A\')\"");
	addOption("connection.host").setType(QMetaType::QString).setNames("-s", "--host").setDefaultValue("localhost");
	addOption("connection.port").setType(QMetaType::Int).setNames("--port").setDefaultValue(5432);
	addOption("connection.user").setType(QMetaType::QString).setNames("-u", "--user").setDefaultValue("quickevent");
	addOption("connection.password").setType(QMetaType::QString).setNames("-p", "--password");
	addOption("connection.database").setType(QMetaType::QString).setNames("-b", "--database").setDefaultValue("quickevent");
	addOption("connection.driver").setType(QMetaType::QString).setNames("--sql-driver").setDefaultValue("QPSQL");
}
