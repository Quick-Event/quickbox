#ifndef CARDREADOUTPLUGIN_H
#define CARDREADOUTPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

class CardReadoutPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CardReadoutPlugin(QObject *parent = nullptr);
private:
	void onInstalled();
};

#endif // CARDREADOUTPLUGIN_H
