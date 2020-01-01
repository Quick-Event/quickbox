#ifndef EXAMPLECPPQML_H
#define EXAMPLECPPQML_H

#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/ipersistentsettings.h>

class ExampleCppQml : public qf::qmlwidgets::framework::Plugin, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	ExampleCppQml(QObject *parent = nullptr);
private:
	void onInstalled();
	void doDbVacuum();
};


#endif
