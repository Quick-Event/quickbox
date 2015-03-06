#include "ogitemdelegate.h"
#include "ogtimeedit.h"

#include <QItemEditorFactory>

OGItemDelegate::OGItemDelegate(QObject *parent)
	: Super(parent)
{
	m_creator = new QStandardItemEditorCreator<OGTimeEdit>();
	QItemEditorFactory *fact = itemEditorFactory();
	fact->registerEditor(qMetaTypeId<OGTimeMs>(), m_creator);
}

OGItemDelegate::~OGItemDelegate()
{
	delete m_creator;
}

