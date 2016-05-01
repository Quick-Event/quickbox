#ifndef SPEAKERPLUGIN_H
#define SPEAKERPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/ipersistentsettings.h>

class Speaker : public qf::qmlwidgets::framework::Plugin, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	Speaker(QObject *parent = nullptr);
private:
	void onInstalled();
// 	void doDbVacuum();
};


#endif
