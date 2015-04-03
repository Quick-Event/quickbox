#ifndef RECEIPES_RECEIPESPLUGIN_H
#define RECEIPES_RECEIPESPLUGIN_H

#include "../receipespluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

namespace Receipes {

class CardChecker;

class RECEIPESPLUGIN_DECL_EXPORT ReceipesPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<Receipes::CardChecker> cardCheckers READ cardCheckersListProperty)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ReceipesPlugin(QObject *parent = nullptr);

	const QList<Receipes::CardChecker*>& cardCheckers() {return m_cardCheckers;}
private:
	void onInstalled();
	QQmlListProperty<Receipes::CardChecker> cardCheckersListProperty();
private:
	QList<Receipes::CardChecker*> m_cardCheckers;
};

}

#endif
