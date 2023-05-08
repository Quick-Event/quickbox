#ifndef CARDREADERSETTINGS_H
#define CARDREADERSETTINGS_H

#include <plugins/Core/src/settings.h>

class CardReaderSettings : public Core::Settings
{
public:
	enum class ReaderMode { Readout, EditOnPunch };
public:
	CardReaderSettings() : Core::Settings("plugin/CardReader") {}

	SETTINGS_FIELD(QString, d, setD, evice, "comm/connection/device", {});
	SETTINGS_FIELD(int, b, setB, audRate, "comm/connection/baudRate", 38400);
	SETTINGS_FIELD(int, d, setD, ataBits, "comm/connection/dataBits", 8);
	SETTINGS_FIELD(int, s, setS, topBits, "comm/connection/stopBits", 1);
	SETTINGS_FIELD(QString, p, setP, arity, "comm/connection/parity", "None");
	SETTINGS_FIELD(bool, isS, setS, howRawComData, "comm/debug/showRawComData", false);
	SETTINGS_FIELD(bool, isD, setD, isableCRCCheck, "comm/debug/disableCRCCheck", false);
	SETTINGS_FIELD1(QString, c, setC, ardCheckType, "Classic");
	SETTINGS_FIELD1(QString, r, setR, eaderMode, "Readout");

	ReaderMode readerModeEnum() const;
};

#endif // CARDREADERSETTINGS_H
