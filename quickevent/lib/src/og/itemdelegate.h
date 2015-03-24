#ifndef QUICKEVENT_OG_ITEMDELEGATE_H
#define QUICKEVENT_OG_ITEMDELEGATE_H

#include "../quickeventglobal.h"

#include <QStyledItemDelegate>

class QItemEditorCreatorBase;

namespace quickevent {
namespace og {

class QUICKEVENT_DECL_EXPORT ItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
private:
	typedef QStyledItemDelegate Super;
public:
	ItemDelegate(QObject * parent = nullptr);
	~ItemDelegate() Q_DECL_OVERRIDE;
private:
	QItemEditorCreatorBase *m_creator = nullptr;
	QItemEditorFactory *m_factory = nullptr;
};

}}

#endif // QUICKEVENT_OG_ITEMDELEGATE_H
