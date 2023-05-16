#ifndef QUICKEVENTGUI_AUDIO_PLAYER_H
#define QUICKEVENTGUI_AUDIO_PLAYER_H

#include "../quickeventguiglobal.h"

#include <QObject>

#include <QMediaPlayer>
#include <QObject>
#include <QQueue>

namespace quickevent {
namespace gui {
namespace audio {

class WavFile;

class QUICKEVENTGUI_DECL_EXPORT Player : public QObject
{
	Q_OBJECT

public:
	enum class AlertKind {Error, Warning, Info, OperatorNotify, OperatorWakeUp};
public:
	Player(QObject* parent = nullptr);
	void playAlert(AlertKind kind);
	void playFile(const QString &file);
	bool isPlaying() const;

private:
	void onPlaybackStateChanged(QMediaPlayer::PlaybackState new_state);
	void playNext();

	QQueue<QString> m_playlist;
	QAudioOutput *m_audioOutput = nullptr;
	QMediaPlayer *m_player = nullptr;
};

}}}

#endif
