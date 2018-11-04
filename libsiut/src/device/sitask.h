#pragma once

#include "../siutglobal.h"
#include "../sicard.h"

#include <qf/core/utils.h>

#include <QObject>

class QTimer;

namespace siut {

class SIMessageData;

class SIUT_DECL_EXPORT SiTask : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	SiTask(QObject *parent = nullptr);
	~SiTask() override;

	enum class Type {Invalid=0, CardRead, Punch, Other};

	Q_SIGNAL void sigSendCommand(int cmd, const QByteArray &data);
	virtual void onSiMessageReceived(const SIMessageData &msg) = 0;
	virtual void start() = 0;
	virtual Type type() const = 0;
	virtual void finishAndDestroy(bool ok, QVariant result);
	void abort() {finishAndDestroy(false, QVariant());}
	Q_SIGNAL void aboutToFinish();
	Q_SIGNAL void finished(bool ok, QVariant result);
protected:
	//void restartRxTimer();
	void sendCommand(int cmd, const QByteArray &data);
protected:
	QTimer *m_rxTimer;
};

class SIUT_DECL_EXPORT SiTaskSetDirectRemoteMode : public SiTask
{
	Q_OBJECT
	using Super = SiTask;
public:
	enum class Mode {Direct = 0, Remote};
public:
	explicit SiTaskSetDirectRemoteMode(Mode mode, QObject *parent = nullptr);

	Type type() const override {return  Type::Other;}
	void start() override;
	void onSiMessageReceived(const siut::SIMessageData &msg) override;
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

class SIUT_DECL_EXPORT SiTaskStationConfig : public SiTask
{
	Q_OBJECT
	using Super = SiTask;
public:
	explicit SiTaskStationConfig(QObject *parent = nullptr) : Super(parent) {}

	Type type() const override {return  Type::Other;}
	void start() override;
	void onSiMessageReceived(const siut::SIMessageData &msg) override;
private:
	enum class State {};
	QByteArray m_data;
};

class SIUT_DECL_EXPORT SiTaskReadStationBackupMemory : public SiTask
{
	Q_OBJECT
	using Super = SiTask;
public:
	explicit SiTaskReadStationBackupMemory(QObject *parent = nullptr);

	Type type() const override {return  Type::Other;}
	void start() override;
	void onSiMessageReceived(const siut::SIMessageData &msg) override;
private:
	Q_SIGNAL void siMessageForwarded(const siut::SIMessageData &msg);
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

class SIUT_DECL_EXPORT SiTaskReadCard : public SiTask
{
	Q_OBJECT
	using Super = SiTask;
public:
	explicit SiTaskReadCard(bool with_autosend, QObject *parent = nullptr)
		: Super(parent)
		, m_withAutosend(with_autosend) {}
	//~SiTaskReadCard() override {}
	Type type() const override {return  Type::CardRead;}
protected:
	bool m_withAutosend;
	SICard m_card;
};

class SIUT_DECL_EXPORT SiTaskReadCard5 : public SiTaskReadCard
{
	Q_OBJECT
	using Super = SiTaskReadCard;
public:
	explicit SiTaskReadCard5(bool with_autosend, QObject *parent = nullptr)
		: Super(with_autosend, parent) {}

	void start() override;
	void onSiMessageReceived(const siut::SIMessageData &msg) override;
};

class SIUT_DECL_EXPORT SiTaskReadCard8 : public SiTaskReadCard
{
	Q_OBJECT
	using Super = SiTaskReadCard;
public:
	explicit SiTaskReadCard8(bool with_autosend, QObject *parent = nullptr)
		: Super(with_autosend, parent) {}
	~SiTaskReadCard8() override;

	void start() override;
	void onSiMessageReceived(const siut::SIMessageData &msg) override;
private:
	enum CardSerie {Invalid = 0, Card8 = 2, Card9 = 1, pCard = 4, tCard = 6, Siac = 15};
	CardSerie m_cardSerie = Invalid;
	int m_punchCnt = 0;
};
/*
class SIUT_DECL_EXPORT SiTaskReadSiac : public SiTaskReadCard
{
	Q_OBJECT
	using Super = SiTaskReadCard;
public:
	explicit SiTaskReadSiac(bool with_autosend, QObject *parent = nullptr)
		: Super(with_autosend, parent) {}
	~SiTaskReadSiac() override;

	void start() override;
	void onSiMessageReceived(const siut::SIMessageData &msg) override;
private:
	enum CardSerie {Invalid = 0, Card8 = 2, Card9 = 1, pCard = 4, tCard = 6, Siac = 15};
	CardSerie m_cardSerie = Invalid;
	int m_punchCnt = 0;
};
*/

}
