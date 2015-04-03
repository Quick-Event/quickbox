#ifndef RECEIPES_RECEIPESPLUGIN_H
#define RECEIPES_RECEIPESPLUGIN_H

#include "../receipespluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

namespace CardReader {
class CardReaderPlugin;
}

namespace Receipes {

class CardChecker;

class RECEIPESPLUGIN_DECL_EXPORT ReceipesPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ReceipesPlugin(QObject *parent = nullptr);

	Q_INVOKABLE void previewReceipe(int card_id);

	Q_INVOKABLE QVariantMap receipeTablesData(int card_id);
private:
	void onInstalled();
	CardReader::CardReaderPlugin* cardReaderPlugin();
private:
};

}

#endif
