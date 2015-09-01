#ifndef RECEIPTS_RECEIPTSPLUGIN_H
#define RECEIPTS_RECEIPTSPLUGIN_H

#include "../receiptspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

class QPrinterInfo;
class QDomElement;

namespace CardReader {
class CardReaderPlugin;
}
namespace Event {
class EventPlugin;
}

class ReceiptsPrinterOptions;
class ReceiptsPrinter;

namespace Receipts {

class RECEIPTSPLUGIN_DECL_EXPORT ReceiptsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ReceiptsPlugin(QObject *parent = nullptr);

	static const QLatin1String SETTINGS_PREFIX;

	Q_INVOKABLE void previewReceipt(int card_id);
	Q_INVOKABLE bool printReceipt(int card_id);

	Q_INVOKABLE QVariantMap receiptTablesData(int card_id);

	void setReceiptsPrinterOptions(const ReceiptsPrinterOptions &opts);
private:
	void onInstalled();
	CardReader::CardReaderPlugin* cardReaderPlugin();
	Event::EventPlugin* eventPlugin();

	ReceiptsPrinterOptions receiptsPrinterOptions();
	ReceiptsPrinter* receiptsPrinter();

	void previewReceipt_classic(int card_id);
	void printReceipt_classic(int card_id);
	QList<QByteArray> createPrinterData(const QDomElement &body, const ReceiptsPrinterOptions &printer_options);

	class DirectPrintContext;
	void createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context);
private:
	ReceiptsPrinter *m_receiptsPrinter = nullptr;
};

}

#endif
