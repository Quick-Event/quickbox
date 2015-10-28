#include "player.h"
#include "wavfile.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QCoreApplication>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QString>

namespace quickevent {
namespace audio {

Player::Player(QObject *parent)
	: QObject(parent)
{
}

void Player::playAlert(AlertKind kind)
{
	QString fn;
	switch(kind) {
	case AlertKind::Error: fn = QStringLiteral("error.wav"); break;
	case AlertKind::Warning: fn = QStringLiteral("warning.wav"); break;
	case AlertKind::Info: fn = QStringLiteral("info.wav"); break;
	case AlertKind::OperatorWakeUp: fn = QStringLiteral("operator-wakeup.wav"); break;
	case AlertKind::OperatorNotify: fn = QStringLiteral("operator-notify.wav"); break;
	}
	if(!fn.isEmpty()) {
		fn = QCoreApplication::applicationDirPath() + "/quickevent-data/style/sound/" + fn;
		playFile(fn);
	}
}

void Player::playFile(const QString &file)
{
	if (m_audioOutput) {
		m_audioOutput->stop();
	}

	if(!m_wavFile)
		m_wavFile = new WavFile(this);
	if(!m_wavFile->open(file)) {
		qfError() << "Cannot open audio file" << file;
		return;
	}

	QF_SAFE_DELETE(m_audioOutput);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(m_wavFile->fileFormat())) {
		qfWarning() << "Raw audio format not supported by backend, cannot play audio.";
		return;
	}

	m_audioOutput = new QAudioOutput(m_wavFile->fileFormat(), this);
	connect(m_audioOutput, &QAudioOutput::stateChanged, [this](QAudio::State new_state) {
		qfDebug() << "Player state changed to" << new_state;
		switch (new_state) {
		case QAudio::IdleState:
			// Finished playing (no more data)
			this->m_audioOutput->stop();
			if(this->m_wavFile)
				this->m_wavFile->close();
			break;

		default:
			// ... other cases as appropriate
			break;
		}
	});

	m_audioOutput->start(m_wavFile);
}

}}

