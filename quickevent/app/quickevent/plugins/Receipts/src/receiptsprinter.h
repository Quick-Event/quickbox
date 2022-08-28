#ifndef RECEIPTSPRINTER_H
#define RECEIPTSPRINTER_H

#include <QObject>

class QDomElement;
class DirectPrintContext;
class ReceiptsSettings;

class ReceiptsPrinter : public QObject
{
	Q_OBJECT
public:
	explicit ReceiptsPrinter(QObject *parent = nullptr);

	bool printReceipt(const QString &report_file_name, const QVariantMap &report_data, int card_id);
private:
	QList<QByteArray> createPrinterData(const QDomElement &body, const ReceiptsSettings &receipts_settings);
	void createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context, const QString &text_encoding);
};

#endif // RECEIPTSPRINTER_H
