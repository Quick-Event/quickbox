#include "itemdelegate.h"
#include "timeedit.h"
#include "siid.h"

#include <QItemEditorFactory>
#include <QSpinBox>

namespace quickevent {
namespace og {

namespace {
class SpinBox : public QSpinBox
{
	Q_OBJECT

	Q_PROPERTY(quickevent::og::SiId siid READ siid WRITE setSiid USER true)
private:
	typedef QSpinBox Super;
public:
	SpinBox(QWidget *parent = nullptr)
		: Super(parent)
	{
		setMaximum(999999999);
		setMinimum(0);
	}

	SiId siid() const {return SiId(value());}
	void setSiid(SiId id) {setValue((int)id);}
};
}

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
		auto creator = new QStandardItemEditorCreator<SpinBox>();
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

#include "itemdelegate.moc"
