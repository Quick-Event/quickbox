#ifndef QUICKEVENT_OG_SIIDEDIT_H
#define QUICKEVENT_OG_SIIDEDIT_H

#include "../quickeventglobal.h"
#include "siid.h"

#include <qf/qmlwidgets/spinbox.h>

namespace quickevent {
namespace og {

class QUICKEVENT_DECL_EXPORT SiIdEdit : public qf::qmlwidgets::SpinBox
{
	Q_OBJECT

	Q_PROPERTY(quickevent::og::SiId siid READ siid WRITE setSiid USER true)
private:
	typedef qf::qmlwidgets::SpinBox Super;
public:
	SiIdEdit(QWidget *parent = nullptr);

	SiId siid() const {return SiId(value());}
	void setSiid(SiId id) {setValue((int)id);}
};

} // namespace og
} // namespace quickevent

#endif // QUICKEVENT_OG_SIIDEDIT_H
