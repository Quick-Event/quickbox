#ifndef EVENT_STAGE_H
#define EVENT_STAGE_H

#include "eventpluginglobal.h"

#include <qf/core/utils.h>

#include <QVariantMap>
#include <QTime>
#include <QDate>

namespace Event {

class EVENTPLUGIN_DECL_EXPORT StageDocument;

class EVENTPLUGIN_DECL_EXPORT StageData : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD(int, i, setI, d)
	//QF_VARIANTMAP_FIELD(QTime, s, setS, tartTime)
	QF_VARIANTMAP_FIELD(bool, is, set, UseAllMaps)
	QF_VARIANTMAP_FIELD(QDateTime, s, setS, tartDateTime)
	QF_VARIANTMAP_FIELD(QVariantMap, d, setD, rawingConfig)
public:
	StageData(const QVariantMap &data = QVariantMap());
	StageData(const StageDocument *doc);
};

}

#endif
