#include "siidedit.h"

namespace quickevent {
namespace og {

SiIdEdit::SiIdEdit(QWidget *parent)
	: Super(parent)
{
	setMaximum(999999999);
	setMinimum(0);
}

} // namespace og
} // namespace quickevent

