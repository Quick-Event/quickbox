#include "action.h"

using namespace qf::qmlwidgets;

Action::Action(QObject *parent) :
	QAction(parent)
{
}

void Action::setText(const QString &new_text)
{
	Super::setText(new_text);
	emit textChanged(new_text);
}
