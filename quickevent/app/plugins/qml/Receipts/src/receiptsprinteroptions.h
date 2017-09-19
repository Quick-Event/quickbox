#ifndef RECEIPTSPRINTEROPTIONS_H
#define RECEIPTSPRINTEROPTIONS_H

#include <qf/core/utils.h>

#include <QVariantMap>
#include <QCoreApplication>

class ReceiptsPrinterOptions : public QVariantMap
{
	Q_DECLARE_TR_FUNCTIONS(ReceiptsPrinterOptions)
public:
	struct PrinterType { enum Enum { GraphicPrinter,  CharacterPrinter}; };
	struct CharacterPrinteType { enum Enum { LPT, Directory, Network, }; };

	QF_VARIANTMAP_FIELD(int, p, setP, rinterType)
	QF_VARIANTMAP_FIELD(QString, g, setG, raphicsPrinterName)
	QF_VARIANTMAP_FIELD2(QString, c, setC, haracterPrinterDevice, QStringLiteral("/dev/usb/lp1"))
	QF_VARIANTMAP_FIELD(QString, c, setC, haracterPrinterDirectory)
	QF_VARIANTMAP_FIELD2(QString, c, setC, haracterPrinterModel, QStringLiteral("Epson TM-T88V"))
	QF_VARIANTMAP_FIELD2(int, c, setC, haracterPrinterLineLength, 41)
	QF_VARIANTMAP_FIELD2(bool, is, set, CharacterPrinterGenerateControlCodes, true)
	QF_VARIANTMAP_FIELD(int, c, setC, haracterPrinterType)
	QF_VARIANTMAP_FIELD(QString, c, setC, haracterPrinterAddress)
	QF_VARIANTMAP_FIELD(QString, c, setC, haracterPrinterCodec)
public:
	explicit ReceiptsPrinterOptions(const QVariantMap &o = QVariantMap());

	QString printerCaption() const;

	static ReceiptsPrinterOptions fromJson(const QByteArray &json);
	QByteArray toJson() const;
};

#endif // RECEIPTSPRINTEROPTIONS_H
