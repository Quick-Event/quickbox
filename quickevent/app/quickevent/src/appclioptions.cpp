#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("locale").setType(QVariant::String).setNames("--locale").setComment(tr("Application locale")).setDefaultValue("system");
	addOption("profile").setType(QVariant::String).setNames("--profile").setComment(tr("Application profile, see: https://github.com/fvacek/quickbox/wiki/Application-profiles"));
	addOption("app.fontScale").setType(QVariant::Double).setNames("--font-scale").setComment(tr("Application font scale")).setDefaultValue(1);
}
