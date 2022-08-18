#include "splitter.h"

#include <qf/core/log.h>

#include <QSettings>

using namespace qf::qmlwidgets;

Splitter::Splitter(QWidget *parent) :
	QSplitter(parent), framework::IPersistentSettings(this)
{
}

Splitter::~Splitter()
{
	savePersistentSettings();
}

void Splitter::loadPersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		QByteArray ba = settings.value("sizes").toByteArray();
		if(!ba.isEmpty()) {
			this->restoreState(ba);
		}
	}
}

void Splitter::savePersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		settings.setValue("sizes", saveState());
	}
}

QQmlListProperty<QWidget> Splitter::widgets()
{
	return QQmlListProperty<QWidget>(this,0,
									 Splitter::addWidgetFunction,
									 Splitter::countWidgetsFunction,
									 Splitter::widgetAtFunction,
									 Splitter::removeAllWidgetsFunction
									 );
}

void Splitter::addWidgetFunction(QQmlListProperty<QWidget> *list_property, QWidget *widget)
{
	if (widget) {
		Splitter *that = static_cast<Splitter*>(list_property->object);
		widget->setParent(0);
		widget->setParent(that);
		widget->show();
		that->addWidget(widget);
	}
}

QWidget * Splitter::widgetAtFunction(QQmlListProperty<QWidget> *list_property, WidgetIndexType index)
{
	Splitter *that = static_cast<Splitter*>(list_property->object);
	return that->widget(index);
}


void Splitter::removeAllWidgetsFunction(QQmlListProperty<QWidget> *list_property)
{
	Splitter *that = static_cast<Splitter*>(list_property->object);
	QList<QWidget*> lst = that->findChildren<QWidget*>();
	qDeleteAll(lst);}

Splitter::WidgetIndexType Splitter::countWidgetsFunction(QQmlListProperty<QWidget> *list_property)
{
	Splitter *that = static_cast<Splitter*>(list_property->object);
	return that->count();
}
