#ifndef CARDREADERPLUGIN_H
#define CARDREADERPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

class CardReaderPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CardReaderPlugin(QObject *parent = nullptr);
private:
	void onInstalled();
};

#endif // CARDREADERPLUGIN_H
