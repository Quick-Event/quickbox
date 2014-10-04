#include "sheet.h"

using namespace qf::qmlwidgets::reports::style;

Sheet::Sheet(QObject *parent)
	: QObject(parent)
{
	qfLogFuncFrame();
}

Sheet::~Sheet()
{
	qfLogFuncFrame();
	//qDeleteAll(m_colors); it seems that QQmlListProperty itself handles color children parentship
}

QQmlListProperty<Color> Sheet::colors()
{
	return QQmlListProperty<Color>(this, m_colors);
}
