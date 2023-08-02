#include "player.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QCoreApplication>
#include <QAudioOutput>

namespace quickevent {
namespace gui {
namespace audio {

Player::Player(QObject *parent)
	: QObject(parent)
{
}

void Player::playAlert(AlertKind kind)
{
	QString fn;
	switch(kind) {
	case AlertKind::Error: fn = QStringLiteral("broken-glass.wav"); break;
	case AlertKind::Warning: fn = QStringLiteral("buzz.wav"); break;
	case AlertKind::Info: fn = QStringLiteral("ding.wav"); break;
	case AlertKind::OperatorWakeUp: fn = QStringLiteral("buzz.wav"); break;
	case AlertKind::OperatorNotify: fn = QStringLiteral("dingding.wav"); break;
	}
	if(!fn.isEmpty()) {
		fn = ":/quickeventgui/sound/" + fn;
		playFile(fn);
	}
}

void Player::playFile(const QString &file)
{
	m_playlist.enqueue(file);
	if (!isPlaying()) {
		playNext();
	}
}

bool Player::isPlaying() const
{
	return m_player && m_player->playbackState() != QMediaPlayer::StoppedState;
}

void Player::playNext()
{
	if (m_playlist.isEmpty()) {
		return;
	}

	//if(Application::instance()->isSilentMode()) {
	//	m_playlist.clear();
	//	return;
	//}

	if (!m_audioOutput) {
		m_audioOutput = new QAudioOutput;
		m_player = new QMediaPlayer;
		m_player->setAudioOutput(m_audioOutput);
		connect(m_player, &QMediaPlayer::playbackStateChanged, this, &Player::onPlaybackStateChanged, Qt::QueuedConnection);
		m_audioOutput->setVolume(100);
	}

	QString file = m_playlist.dequeue();
	if (file.startsWith(':')) {
		m_player->setSource(QUrl("qrc" + file));
	}
	else {
		m_player->setSource(QUrl::fromLocalFile(file));
	}
	m_player->setPosition(0);
	m_player->play();
}

void Player::onPlaybackStateChanged(QMediaPlayer::PlaybackState new_state)
{
	if (new_state == QMediaPlayer::StoppedState) {
		playNext();
	}
}

}}}

