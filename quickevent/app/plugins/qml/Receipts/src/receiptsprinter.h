#ifndef RECEIPTSPRINTER_H
#define RECEIPTSPRINTER_H

#include "receiptsprinteroptions.h"

#include <QObject>

class QDomElement;
class DirectPrintContext;

class ReceiptsPrinter : public QObject
{
	Q_OBJECT
public:
	explicit ReceiptsPrinter(const ReceiptsPrinterOptions &opts,  QObject *parent = 0);

	void printReceipt(const QString &report_file_name, const QVariantMap &report_data);
private:
	QList<QByteArray> createPrinterData(const QDomElement &body, const ReceiptsPrinterOptions &printer_options);
	void createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context);
private:
	ReceiptsPrinterOptions m_printerOptions;
};

#endif // RECEIPTSPRINTER_H
