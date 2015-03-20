#ifndef QUICKEVENT_PARTWIDGET_H
#define QUICKEVENT_PARTWIDGET_H

#include "../quickeventglobal.h"

#include <qf/qmlwidgets/framework/partwidget.h>

namespace quickevent {

class QUICKEVENT_DECL_EXPORT PartWidget : public qf::qmlwidgets::framework::PartWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::PartWidget Super;
public:
	PartWidget(QWidget *parent = 0);

	Q_SIGNAL void reloadRequest();
	//Q_SIGNAL void resetRequest();
protected:
	//virtual void reload() {}

	Q_SLOT virtual void onActiveChanged();
};

}

#endif // QUICKEVENT_PARTWIDGET_H
