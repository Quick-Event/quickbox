#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("locale").setType(QMetaType::QString).setNames("--locale").setComment(tr("Application locale")).setDefaultValue("system");
	addOption("profile").setType(QMetaType::QString).setNames("--profile").setComment(tr("Application profile, see: https://github.com/fvacek/quickbox/wiki/Application-profiles"));
	addOption("app.fontScale").setType(QMetaType::Double).setNames("--font-scale").setComment(tr("Application font scale")).setDefaultValue(1);
}
