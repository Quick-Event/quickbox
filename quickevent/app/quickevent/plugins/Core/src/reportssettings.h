#ifndef REPORTSSETTINGS_H
#define REPORTSSETTINGS_H

#include "settings.h"

class ReportsSettings : public Core::Settings
{
public:
	ReportsSettings() : Core::Settings("plugin/Core/reports") {}

	SETTINGS_FIELD(QString, c, setC, ustomReportsDirectory, "customReportsDirectory", {});
	SETTINGS_FIELD(QString, c, setC, urrentReceipt, "currentReceipt", {});
	SETTINGS_FIELD(QString, r, setR, eceiptPrinterOptions, "receiptPrinterOptions", {});
};

#endif // REPORTSSETTINGS_H
