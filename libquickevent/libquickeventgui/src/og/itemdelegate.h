#ifndef QUICKEVENTGUI_OG_ITEMDELEGATE_H
#define QUICKEVENTGUI_OG_ITEMDELEGATE_H

#include "../quickeventguiglobal.h"

#include <qf/qmlwidgets/sqltableitemdelegate.h>

class QItemEditorCreatorBase;

namespace quickevent {
namespace gui {
namespace og {

class QUICKEVENTGUI_DECL_EXPORT ItemDelegate : public qf::qmlwidgets::SqlTableItemDelegate
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::SqlTableItemDelegate Super;
public:
	ItemDelegate(qf::qmlwidgets::TableView * parent = nullptr);
	~ItemDelegate() Q_DECL_OVERRIDE;
private:
	QItemEditorFactory *m_factory = nullptr;
};

}}}

#endif // QUICKEVENTGUI_OG_ITEMDELEGATE_H
