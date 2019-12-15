#ifndef CLASSES_CLASSDOCUMENT_H
#define CLASSES_CLASSDOCUMENT_H

#include "classespluginglobal.h"

#include <qf/core/model/sqldatadocument.h>

namespace Classes {

class CLASSESPLUGIN_DECL_EXPORT ClassDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	ClassDocument(QObject *parent = nullptr);
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
};

}

#endif // CLASSDOCUMENT_H
