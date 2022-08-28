#ifndef RECEIPTS_RECEIPTSPLUGIN_H
#define RECEIPTS_RECEIPTSPLUGIN_H

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

class ReceiptsPrinter;
class ReceiptsSettings;

namespace Receipts {

class ReceiptsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ReceiptsPlugin(QObject *parent = nullptr);

	void previewCard(int card_id);
	void previewReceipt(int card_id);
	bool printReceipt(int card_id);
	bool printCard(int card_id);
	bool printError(int card_id);
	void printOnAutoPrintEnabled(int card_id);

	QVariantMap readCardTablesData(int card_id);
	QVariantMap receiptTablesData(int card_id);

	ReceiptsPrinter* receiptsPrinter();

	bool isAutoPrintEnabled();
private:
	void onInstalled();

	QList<QByteArray> createPrinterData(const QDomElement &body, const ReceiptsSettings &receipts_settings);

	class DirectPrintContext;
	void createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context);
private:
	ReceiptsPrinter *m_receiptsPrinter = nullptr;
};

}

#endif
