#ifndef CARDREADERPLUGIN_H
#define CARDREADERPLUGIN_H

#include "cardreaderpluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

namespace CardReader {
class  CardChecker;
}

class CARDREADERPLUGIN_DECL_EXPORT CardReaderPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<CardReader::CardChecker> cardCheckers READ cardCheckersListProperty)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CardReaderPlugin(QObject *parent = nullptr);

	const QList<CardReader::CardChecker*>& cardCheckers() {return m_cardCheckers;}
private:
	void onInstalled();
	QQmlListProperty<CardReader::CardChecker> cardCheckersListProperty();
private:
	QList<CardReader::CardChecker*> m_cardCheckers;
};

#endif // CARDREADERPLUGIN_H
