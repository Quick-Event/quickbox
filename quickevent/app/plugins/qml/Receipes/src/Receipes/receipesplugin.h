#ifndef RECEIPES_RECEIPESPLUGIN_H
#define RECEIPES_RECEIPESPLUGIN_H

#include "../receipespluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

class QPrinterInfo;

namespace CardReader {
class CardReaderPlugin;
}
namespace Event {
class EventPlugin;
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
	Q_INVOKABLE bool printReceipe(int card_id, const QPrinterInfo &printer_info);

	Q_INVOKABLE QVariantMap receipeTablesData(int card_id);
private:
	void onInstalled();
	CardReader::CardReaderPlugin* cardReaderPlugin();
	Event::EventPlugin* eventPlugin();

	void previewReceipe_classic(int card_id);
	void printReceipe_classic(int card_id, const QPrinterInfo &printer_info);
};

}

#endif
