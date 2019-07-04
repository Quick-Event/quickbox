#include "siidedit.h"

namespace quickevent {
namespace gui {
namespace si {

SiIdEdit::SiIdEdit(QWidget *parent)
	: Super(parent)
{
	setMaximum(999999999);
	setMinimum(0);
}

}}}
