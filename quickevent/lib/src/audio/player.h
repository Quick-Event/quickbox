#ifndef QUICKEVENT_AUDIO_PLAYER_H
#define QUICKEVENT_AUDIO_PLAYER_H

#include "../quickeventglobal.h"

#include <QObject>

class QAudioOutput;

namespace quickevent {
namespace audio {

class WavFile;

class QUICKEVENT_DECL_EXPORT Player : public QObject
{
	Q_OBJECT
public:
	enum class AlertKind {Error, Warning, Info, OperatorNotify, OperatorWakeUp};
public:
	Player(QObject* parent = nullptr);
	//~Player() Q_DECL_OVERRIDE;

	void playAlert(AlertKind kind);
	void playFile(const QString& file);
private:
	QAudioOutput* m_audioOutput = nullptr;
	WavFile *m_wavFile = nullptr;
};

}}

#endif
