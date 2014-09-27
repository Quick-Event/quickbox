#include "cursoroverrider.h"

#include <QApplication>

using namespace qf::qmlwidgets::framework;

CursorOverrider::CursorOverrider(Qt::CursorShape cursor_shape)
{
	QApplication::setOverrideCursor(cursor_shape);
}

CursorOverrider::~CursorOverrider()
{
	QApplication::restoreOverrideCursor();
}
