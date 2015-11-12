#include "itemdelegate.h"
#include "timeedit.h"
#include "siid.h"
#include "siidedit.h"

#include <QItemEditorFactory>
#include <QSpinBox>

namespace quickevent {
namespace og {

ItemDelegate::ItemDelegate(qf::qmlwidgets::TableView *parent)
	: Super(parent)
{
	QItemEditorFactory *fact = itemEditorFactory();
	if(!fact) {
		fact = m_factory = new QItemEditorFactory();
		setItemEditorFactory(m_factory);
	}
	{
		auto creator = new QStandardItemEditorCreator<TimeEdit>();
		fact->registerEditor(qMetaTypeId<TimeMs>(), creator);
	}
	{
		auto creator = new QStandardItemEditorCreator<SiIdEdit>();
		fact->registerEditor(qMetaTypeId<SiId>(), creator);
	}
}

ItemDelegate::~ItemDelegate()
{
	//QF_SAFE_DELETE(m_creator); The factory takes ownership of the item editor
	QF_SAFE_DELETE(m_factory);
}

}
}

//#include "itemdelegate.moc"
