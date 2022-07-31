#ifndef CARDREADERSETTINGS_H
#define CARDREADERSETTINGS_H

#include <plugins/Core/src/settings.h>

class CardReaderSettings : public Core::Settings
{
public:
	CardReaderSettings() : Core::Settings("plugin/CardReader") {}

	SETTINGS_FIELD(QString, d, setD, evice, "comm/connection/device", {});
	SETTINGS_FIELD(int, b, setB, audRate, "comm/connection/baudRate", 38400);
	SETTINGS_FIELD(int, d, setD, ataBits, "comm/connection/dataBits", 8);
	SETTINGS_FIELD(int, s, setS, topBits, "comm/connection/stopBits", 1);
	SETTINGS_FIELD(QString, p, setP, arity, "comm/connection/parity", "None");
	SETTINGS_FIELD(bool, s, setS, howRawComData, "comm/debug/showRawComData", false);
	SETTINGS_FIELD(bool, d, setD, isableCRCCheck, "comm/debug/disableCRCCheck", false);
};

#endif // CARDREADERSETTINGS_H
