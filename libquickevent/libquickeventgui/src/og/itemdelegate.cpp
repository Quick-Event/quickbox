#include "itemdelegate.h"
#include "timeedit.h"
#include "../si/siidedit.h"

#include <quickevent/core/si/siid.h>

#include <QItemEditorFactory>
#include <QSpinBox>

namespace quickevent {
namespace gui {
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
		fact->registerEditor(qMetaTypeId<core::og::LapTimeMs>(), creator);
	}
	{
		auto creator = new QStandardItemEditorCreator<si::SiIdEdit>();
		fact->registerEditor(qMetaTypeId<core::si::SiId>(), creator);
	}
}

ItemDelegate::~ItemDelegate()
{
	//QF_SAFE_DELETE(m_creator); The factory takes ownership of the item editor
	QF_SAFE_DELETE(m_factory);
}

}}}

