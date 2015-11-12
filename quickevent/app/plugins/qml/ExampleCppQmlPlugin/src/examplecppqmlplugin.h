#ifndef EXAMPLECPPQMLPLUGIN_H
#define EXAMPLECPPQMLPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/ipersistentsettings.h>

class ExampleCppQmlPlugin : public qf::qmlwidgets::framework::Plugin, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ExampleCppQmlPlugin(QObject *parent = nullptr);
private:
	void onInstalled();
	void doDbVacuum();
};


#endif
