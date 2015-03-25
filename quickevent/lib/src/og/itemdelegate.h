#ifndef QUICKEVENT_OG_ITEMDELEGATE_H
#define QUICKEVENT_OG_ITEMDELEGATE_H

#include "../quickeventglobal.h"

#include <qf/qmlwidgets/sqltableitemdelegate.h>

class QItemEditorCreatorBase;

namespace quickevent {
namespace og {

class QUICKEVENT_DECL_EXPORT ItemDelegate : public qf::qmlwidgets::SqlTableItemDelegate
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

}}

#endif // QUICKEVENT_OG_ITEMDELEGATE_H
