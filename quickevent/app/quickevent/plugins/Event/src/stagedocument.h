#ifndef EVENT_STAGEDOCUMENT_H
#define EVENT_STAGEDOCUMENT_H

#include <qf/core/model/sqldatadocument.h>

namespace Event {

class StageDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	StageDocument(QObject *parent = nullptr);
};

}

#endif // EVENT_STAGEDOCUMENT_H
