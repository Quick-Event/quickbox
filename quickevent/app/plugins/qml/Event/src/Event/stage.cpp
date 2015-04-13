#include "stage.h"
#include "stagedocument.h"

#include <QJsonDocument>
#include <QJsonObject>

using namespace Event;

Stage::Stage(const QVariantMap &data)
	: Super(data)
{
}

Event::Stage::Stage(const Event::StageDocument *doc)
{
	for(auto s : {"id", "startTime", "startDate"}) {
		insert(s, doc->value(s));
	}
	QByteArray ba = doc->value("drawingConfig").toString().toUtf8();
	QJsonDocument jsd = QJsonDocument::fromJson(ba);
	insert("drawingConfig", jsd.object().toVariantMap());
}
