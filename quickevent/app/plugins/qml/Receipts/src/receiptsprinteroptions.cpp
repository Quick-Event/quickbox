#include "receiptsprinteroptions.h"

#include <qf/core/log.h>

#include <QJsonDocument>
#include <QJsonObject>

ReceiptsPrinterOptions::ReceiptsPrinterOptions(const QVariantMap &o)
	: QVariantMap(o)
{
}

QString ReceiptsPrinterOptions::printerCaption() const
{
	QString ret;
	if(printerType() == (int)PrinterType::GraphicPrinter) {
		ret = tr("Graphics") + ' ' + graphicsPrinterName();
	}
	else {
		ret = tr("Character") + ' ' + characterPrinterModel() + " " + characterPrinterDevice();
	}
	return ret;
}

ReceiptsPrinterOptions ReceiptsPrinterOptions::fromJson(const QByteArray &json)
{
	if(!json.isEmpty()) {
		QJsonParseError err;
		QJsonDocument jsd = QJsonDocument::fromJson(json, &err);
		if(err.error == QJsonParseError::NoError) {
			QVariantMap m = jsd.object().toVariantMap();
			return ReceiptsPrinterOptions(m);
		}
		qfError() << "Error parsing ReceiptsPrinterSettings JSON:" << err.errorString();
	}
	return ReceiptsPrinterOptions();
}

QByteArray ReceiptsPrinterOptions::toJson() const
{
	QJsonDocument jsd = QJsonDocument::fromVariant(*this);
	return jsd.toJson(QJsonDocument::Compact);
}


