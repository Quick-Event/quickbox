#ifndef EVENT_STAGEDATA_H
#define EVENT_STAGEDATA_H

#include <QVariantMap>

namespace Event {

class StageData : public QVariantMap
{
private:
	typedef QVariantMap Super;
public:
	StageData(const QVariantMap &data = QVariantMap());
};

}

#endif // EVENT_STAGEDATA_H
