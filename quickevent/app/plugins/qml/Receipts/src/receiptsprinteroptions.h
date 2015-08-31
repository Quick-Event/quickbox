#ifndef RECEIPTSPRINTEROPTIONS_H
#define RECEIPTSPRINTEROPTIONS_H

#include <qf/core/utils.h>

#include <QVariantMap>

class ReceiptsPrinterOptions : public QVariantMap
{
	QF_VARIANTMAP_FIELD(int, p, setP, rinterType)
	QF_VARIANTMAP_FIELD(QString, g, setG, raphicsPrinterName)
	QF_VARIANTMAP_FIELD2(QString, c, setC, haracterPrinterDevice, QStringLiteral("/dev/usb/lp1"))
	QF_VARIANTMAP_FIELD2(QString, c, setC, haracterPrinterModel, QStringLiteral("Epson TM-T88V"))
	QF_VARIANTMAP_FIELD2(int, c, setC, haracterPrinterLineLength, 41)
public:
	enum class PrinterType : int {GraphicsPrinter = 0, CharacterPrinter};
	explicit ReceiptsPrinterOptions(const QVariantMap &o = QVariantMap());
};

#endif // RECEIPTSPRINTEROPTIONS_H
