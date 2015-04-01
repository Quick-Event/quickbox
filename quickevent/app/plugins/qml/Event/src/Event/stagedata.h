#ifndef EVENT_STAGEDATA_H
#define EVENT_STAGEDATA_H

#include <QVariantMap>

namespace Event {

class StageData : public QVariantMap
{
public:
	StageData(const QVariantMap &data = QVariantMap());
};

}

#endif // EVENT_STAGEDATA_H
