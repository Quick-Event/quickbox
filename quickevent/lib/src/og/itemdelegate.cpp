#include "itemdelegate.h"
#include "timeedit.h"

#include <QItemEditorFactory>

using namespace quickevent::og;

ItemDelegate::ItemDelegate(QObject *parent)
	: Super(parent)
{
	m_creator = new QStandardItemEditorCreator<TimeEdit>();
	QItemEditorFactory *fact = itemEditorFactory();
	if(!fact) {
		fact = m_factory = new QItemEditorFactory();
		setItemEditorFactory(m_factory);
	}
	fact->registerEditor(qMetaTypeId<TimeMs>(), m_creator);
}

ItemDelegate::~ItemDelegate()
{
	//QF_SAFE_DELETE(m_creator); The factory takes ownership of the item editor
	QF_SAFE_DELETE(m_factory);
}

