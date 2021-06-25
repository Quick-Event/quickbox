#ifndef QUICKEVENTGUI_PARTWIDGET_H
#define QUICKEVENTGUI_PARTWIDGET_H

#include "quickeventguiglobal.h"
#include <qf/qmlwidgets/framework/partwidget.h>

namespace quickevent {
namespace gui {

class QUICKEVENTGUI_DECL_EXPORT PartWidget : public qf::qmlwidgets::framework::PartWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::PartWidget Super;
public:
	explicit PartWidget(const QString &title, const QString &feature_id, QWidget *parent = nullptr);

	Q_SIGNAL void resetPartRequest();
	Q_SIGNAL void reloadPartRequest();
protected:
	//virtual void reload() {}

	Q_SLOT virtual void onActiveChanged();
};

}}

#endif // QUICKEVENTGUI_PARTWIDGET_H
