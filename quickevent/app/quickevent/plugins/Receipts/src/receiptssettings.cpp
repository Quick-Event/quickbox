#include "receiptssettings.h"

ReceiptsSettings::WhenRunnerNotFoundPrint ReceiptsSettings::whenRunnerNotFoundPrintEnum() const
{
	if(whenRunnerNotFoundPrint() == "ReceiptWithoutName")
		return WhenRunnerNotFoundPrint::ReceiptWithoutName;
	return WhenRunnerNotFoundPrint::ErrorInfo;
}

QString ReceiptsSettings::printerTypeToString(ReceiptsSettings::PrinterType e)
{
	switch(e) {
	case ReceiptsSettings::PrinterType::GraphicPrinter:
		return QStringLiteral("GraphicPrinter");
	case ReceiptsSettings::PrinterType::CharacterPrinter:
		return QStringLiteral("CharacterPrinter");
	}
	return {};
}

QString ReceiptsSettings::characterPrinterTypeToString(ReceiptsSettings::CharacterPrinteType e)
{
	switch(e) {
	case ReceiptsSettings::CharacterPrinteType::LPT:
		return QStringLiteral("LPT");
	case ReceiptsSettings::CharacterPrinteType::Directory:
		return QStringLiteral("Directory");
	case ReceiptsSettings::CharacterPrinteType::Network:
		return QStringLiteral("Network");
	}
	return {};
}

ReceiptsSettings::PrinterType ReceiptsSettings::printerTypeEnum() const
{
	const auto pt = printerType();
	if(pt == "GraphicPrinter")
		return PrinterType::GraphicPrinter;
	return PrinterType::CharacterPrinter;
}

ReceiptsSettings::CharacterPrinteType ReceiptsSettings::characterPrinterTypeEnum() const
{
	const auto pt = characterPrinterType();
	if(pt == "Directory")
		return CharacterPrinteType::Directory;
	if(pt == "Network")
		return CharacterPrinteType::Network;
	return CharacterPrinteType::LPT;
}

QString ReceiptsSettings::printerCaption() const
{
	QString ret;
	if(printerType() == (int)PrinterType::GraphicPrinter) {
		ret = tr("Graphics") + ' ' + graphicsPrinterName();
	}
	else {
		ret = tr("Character") + ' ' + characterPrinterModel() + ' ';
		switch(characterPrinterTypeEnum()) {
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
