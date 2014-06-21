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

QString Action::shortcut() const
{
	QKeySequence ks = Super::shortcut();
	return ks.toString();
}

void Action::setShortcut(const QString &new_text)
{
	QKeySequence ks(new_text);
	if(ks != Super::shortcut()) {
		Super::setShortcut(ks);
		emit shortcutChanged(shortcut());
	}
}
