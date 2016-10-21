#ifndef QUICKEVENT_SI_SIIDEDIT_H
#define QUICKEVENT_SI_SIIDEDIT_H

#include "../quickeventglobal.h"
#include "siid.h"

#include <qf/qmlwidgets/spinbox.h>

namespace quickevent {
namespace si {

class QUICKEVENT_DECL_EXPORT SiIdEdit : public qf::qmlwidgets::SpinBox
{
	Q_OBJECT

	Q_PROPERTY(quickevent::si::SiId siid READ siid WRITE setSiid USER true)
private:
	typedef qf::qmlwidgets::SpinBox Super;
public:
	SiIdEdit(QWidget *parent = nullptr);

	SiId siid() const {return SiId(value());}
	void setSiid(SiId id) {setValue((int)id);}
};

}}

#endif // QUICKEVENT_SI_SIIDEDIT_H
