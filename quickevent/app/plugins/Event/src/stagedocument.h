#ifndef EVENT_STAGEDOCUMENT_H
#define EVENT_STAGEDOCUMENT_H

#include "eventpluginglobal.h"

#include <qf/core/model/sqldatadocument.h>

namespace Event {

class EVENTPLUGIN_DECL_EXPORT StageDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	StageDocument(QObject *parent = nullptr);
};

}

#endif // EVENT_STAGEDOCUMENT_H
