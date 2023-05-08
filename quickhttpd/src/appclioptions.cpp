#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("locale").setType(QMetaType::QString).setNames("--locale").setComment("Application locale (e.g. cs_CZ for Czech, en_US for English etc.)").setDefaultValue("system");
	addOption("application.htmlDir").setType(QMetaType::QString).setNames("--html-dir").setDefaultValue("./html").setComment("directory where HTML pages will be stored");
	addOption("application.refreshTime").setType(QMetaType::Int).setNames("--refresh-time").setDefaultValue(60*1000).setComment("refresh time in msec");
	addOption("event.name").setType(QMetaType::QString).setNames("-e", "--event");
	addOption("event.stage").setType(QMetaType::Int).setNames("-n", "--stage").setComment("If not set, the current stage number is loaded from database.");
	addOption("event.classesLike").setType(QMetaType::QString).setNames("--classes-like").setComment("SQL LIKE expression to filter classes to show, for ex. --classes-like \"H%\"");
	addOption("event.classesNotLike").setType(QMetaType::QString).setNames("--classes-not-like").setComment("SQL LIKE expression to filter classes not to show, for ex. --not-classes-like \"HDR\"");
	addOption("sql.host").setType(QMetaType::QString).setNames("-s", "--sql-host").setDefaultValue("localhost");
	addOption("sql.port").setType(QMetaType::Int).setNames("--sql-port").setDefaultValue(5432);
	addOption("sql.user").setType(QMetaType::QString).setNames("-u", "--sql-user").setDefaultValue("quickevent");
	addOption("sql.password").setType(QMetaType::QString).setNames("-p", "--sql-password");
	addOption("sql.database").setType(QMetaType::QString).setNames("-b", "--sql-database").setDefaultValue("quickevent");
	addOption("sql.driver").setType(QMetaType::QString).setNames("--sql-driver").setDefaultValue("QPSQL");

	addOption("http.port").setType(QMetaType::Int).setNames("--http-port").setDefaultValue(8888);
}
