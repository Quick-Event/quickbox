#ifndef QUICKEVENT_AUDIO_WAVFILE_H
#define QUICKEVENT_AUDIO_WAVFILE_H

#include "../quickeventglobal.h"

//#include <QObject>
#include <QFile>
#include <QAudioFormat>

namespace quickevent {
namespace audio {

class QUICKEVENT_DECL_EXPORT WavFile : public QFile
{
public:
	WavFile(QObject *parent = 0);

	using QFile::open;
	bool open(const QString &fileName);
	const QAudioFormat &fileFormat() const;
	qint64 headerLength() const;

private:
	bool readHeader();

private:
	QAudioFormat m_fileFormat;
	qint64 m_headerLength;
};

}}

#endif
