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
protected:
	//virtual void reload() {}

	virtual void onActiveChanged(bool is_active);
};

}

#endif // QUICKEVENT_PARTWIDGET_H
