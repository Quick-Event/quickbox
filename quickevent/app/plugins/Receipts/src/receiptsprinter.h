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
	explicit ReceiptsPrinter(const ReceiptsPrinterOptions &opts,  QObject *parent = nullptr);

	const ReceiptsPrinterOptions& printerOptions() const {return m_printerOptions;}

	bool printReceipt(const QString &report_file_name, const QVariantMap &report_data);
private:
	QList<QByteArray> createPrinterData(const QDomElement &body, const ReceiptsPrinterOptions &printer_options);
	void createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context, const QString &text_encoding);
private:
	ReceiptsPrinterOptions m_printerOptions;
};

#endif // RECEIPTSPRINTER_H
