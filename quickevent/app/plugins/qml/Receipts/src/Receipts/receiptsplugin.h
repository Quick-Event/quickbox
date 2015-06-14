#ifndef RECEIPTS_RECEIPTSPLUGIN_H
#define RECEIPTS_RECEIPTSPLUGIN_H

#include "../receiptspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

class QPrinterInfo;

namespace CardReader {
class CardReaderPlugin;
}
namespace Event {
class EventPlugin;
}

namespace Receipts {

class CardChecker;

class RECEIPTSPLUGIN_DECL_EXPORT ReceiptsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ReceiptsPlugin(QObject *parent = nullptr);

	Q_INVOKABLE void previewReceipt(int card_id);
	Q_INVOKABLE bool printReceipt(int card_id, const QPrinterInfo &printer_info);

	Q_INVOKABLE QVariantMap receiptTablesData(int card_id);
private:
	void onInstalled();
	CardReader::CardReaderPlugin* cardReaderPlugin();
	Event::EventPlugin* eventPlugin();

	void previewReceipt_classic(int card_id);
	void printReceipt_classic(int card_id, const QPrinterInfo &printer_info);
};

}

#endif
