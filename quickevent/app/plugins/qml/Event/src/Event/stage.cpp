#include "stage.h"
#include "stagedocument.h"

#include <QJsonDocument>
#include <QJsonObject>

using namespace Event;

StageData::StageData(const QVariantMap &data)
	: Super(data)
{
}

Event::StageData::StageData(const Event::StageDocument *doc)
{
	const auto START_DATE_TIME = QStringLiteral("startDateTime");
	const auto ID = QStringLiteral("id");
	insert(ID, doc->value(ID));
	QDateTime dt = doc->value(ID).toDateTime();
	/*
	if(doc->isValidFieldName(START_DATE_TIME)) {
		dt = doc->value(ID).toDateTime();
	}
	else {
		// for compatibility with old DB versions
		QDate d = doc->value(QStringLiteral("startDate")).toDate();
		QTime t = doc->value(QStringLiteral("startTime")).toTime();
		dt.setDate(d);
		dt.setTime(t);
	}
	*/
	insert(START_DATE_TIME, dt);
	QByteArray ba = doc->value(QStringLiteral("drawingConfig")).toString().toUtf8();
	QJsonDocument jsd = QJsonDocument::fromJson(ba);
	insert(QStringLiteral("drawingConfig"), jsd.object().toVariantMap());
}
