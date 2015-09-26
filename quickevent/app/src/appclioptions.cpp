#include "appclioptions.h"

AppCliOptions::AppCliOptions(QObject *parent)
	: Super(parent)
{
	addOption("locale").setType(QVariant::String).setNames("--locale").setComment(tr("Application locale")).setDefaultValue("system");
	//addOption("server.host").setType(QVariant::String).setNames("-s", "--server-host").setComment(tr("EYAS server host")).setDefaultValue("localhost");
	//addOption("server.port").setType(QVariant::Int).setNames("-p", "--server-port").setComment(tr("EYAS server port")).setDefaultValue(3745);
}
