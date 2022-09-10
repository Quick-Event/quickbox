#include "receiptsprinteroptions.h"
#if 0

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
		ret = tr("Character") + ' ' + characterPrinterModel() + ' ';
		switch(characterPrinterType()) {
			case CharacterPrinteType::LPT:
				ret +=  characterPrinterDevice();
				break;
			case CharacterPrinteType::Directory:
				ret += characterPrinterDirectory();
				break;
			case CharacterPrinteType::Network:
				ret += characterPrinterUrl();
				break;
		}
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
#endif

