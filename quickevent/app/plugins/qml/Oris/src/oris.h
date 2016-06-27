#ifndef ORIS_H
#define ORIS_H

#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/ipersistentsettings.h>

class Oris : public qf::qmlwidgets::framework::Plugin, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	Oris(QObject *parent = nullptr);
};


#endif
