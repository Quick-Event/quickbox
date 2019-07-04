#ifndef QUICKEVENTGUI_SI_SIIDEDIT_H
#define QUICKEVENTGUI_SI_SIIDEDIT_H

#include "../quickeventguiglobal.h"

#include <quickevent/core/si/siid.h>

#include <qf/qmlwidgets/spinbox.h>

namespace quickevent {
namespace gui {
namespace si {

class QUICKEVENTGUI_DECL_EXPORT SiIdEdit : public qf::qmlwidgets::SpinBox
{
	Q_OBJECT

	Q_PROPERTY(quickevent::core::si::SiId siid READ siid WRITE setSiid USER true)
private:
	typedef qf::qmlwidgets::SpinBox Super;
public:
	SiIdEdit(QWidget *parent = nullptr);

	core::si::SiId siid() const {return core::si::SiId(value());}
	void setSiid(core::si::SiId id) {setValue((int)id);}
};

}}}

#endif // QUICKEVENTGUI_SI_SIIDEDIT_H
