#ifndef RECEIPTS_RECEIPTSPLUGIN_H
#define RECEIPTS_RECEIPTSPLUGIN_H

#include "receiptspluginglobal.h"

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

	Q_INVOKABLE void previewCard(int card_id);
	Q_INVOKABLE void previewReceipt(int card_id);
	Q_INVOKABLE bool printReceipt(int card_id);
	Q_INVOKABLE bool printCard(int card_id);
	Q_INVOKABLE bool printError(int card_id);
	Q_INVOKABLE void printOnAutoPrintEnabled(int card_id);

	QVariantMap readCardTablesData(int card_id);
	Q_INVOKABLE QVariantMap receiptTablesData(int card_id);

	QString currentReceiptPath();
	void setCurrentReceiptPath(const QString &path);

	void setReceiptsPrinterOptions(const ReceiptsPrinterOptions &opts);
	ReceiptsPrinter* receiptsPrinter();

	bool isAutoPrintEnabled();
private:
	void onInstalled();

	ReceiptsPrinterOptions receiptsPrinterOptions();

	void previewReceipt(int card_id, const QString &receipt_path);
	void printReceipt(int card_id, const QString &receipt_path);
	QList<QByteArray> createPrinterData(const QDomElement &body, const ReceiptsPrinterOptions &printer_options);

	class DirectPrintContext;
	void createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context);
private:
	ReceiptsPrinter *m_receiptsPrinter = nullptr;
};

}

#endif
