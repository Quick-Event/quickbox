#pragma once

#include <qf/core/utils.h>

#include <siut/sidevicedriver.h>

#include <QObject>

class SiTaskSetDirectRemoteMode : public siut::SiTask
{
	Q_OBJECT
	using Super = siut::SiTask;
public:
	enum class Mode {Direct = 0, Remote};
public:
	explicit SiTaskSetDirectRemoteMode(Mode mode, QObject *parent = nullptr);

	void start() override;
	void onSiMessageReceived(const QByteArray &msg) override;
private:
	Mode m_mode;
};

class QFCORE_DECL_EXPORT SiStationConfig : public QVariantMap
{
	QF_VARIANTMAP_FIELD2(int, s, setS, tationNumber, 0)
	QF_VARIANTMAP_FIELD2(unsigned, f, setF, lags, 0)

public:
	enum class Flag : unsigned {
				   ExtendedMode = 1 << 0,
				   AutoSend = 1 << 1,
				   HandShake = 1 << 2,
				   PasswordAccess = 1 << 4,
				   ReadOutAfterPunch = 1 << 7,
	};
	SiStationConfig(const QVariantMap &m = QVariantMap()) : QVariantMap(m) {}
};

class SiTaskStationConfig : public siut::SiTask
{
	Q_OBJECT
	using Super = siut::SiTask;
public:
	explicit SiTaskStationConfig(QObject *parent = nullptr) : Super(parent) {}

	void start() override;
	void onSiMessageReceived(const QByteArray &msg) override;
private:
	enum class State {};
	QByteArray m_data;
};

class SiTaskReadStationBackupMemory : public siut::SiTask
{
	Q_OBJECT
	using Super = siut::SiTask;
public:
	explicit SiTaskReadStationBackupMemory(QObject *parent = nullptr);

	void start() override;
	void onSiMessageReceived(const QByteArray &msg) override;
private:
	Q_SIGNAL void siMessageForwarded(const SIMessageData &msg);
private:
	enum class State {SwitchToRemote, ReadPointer, CheckOverflow, ReadData, SwitchToDirect};
	static constexpr unsigned MEMORY_START = 0x100;
	static constexpr unsigned MEMORY_SIZE = 0x200000;
	State m_state = State::SwitchToRemote;
	uint32_t m_memoryDataPointer;
	uint32_t m_readDataPointer;
	unsigned m_blockSize = 128;
	unsigned m_blockCount;
	bool m_isOverflow;
	QByteArray m_data;
};

