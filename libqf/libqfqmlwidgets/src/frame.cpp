#include "frame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <QDebug>

Frame::Frame(QWidget *parent) :
	Super(parent)
{
	Super::setLayout(new QVBoxLayout(this));
	//qDebug() << "adding label" << this;
	//m_layout->addWidget(new QLabel("ahoj", this));
}

Frame::LayoutType Frame::layoutType() const
{
	if(qobject_cast<QVBoxLayout*>(Super::layout())) {
		return LayoutVertical;
	}
	return LayoutHorizontal;
}

void Frame::setLayoutType(Frame::LayoutType ly)
{
	if(ly != layoutType()) {
		QBoxLayout *new_ly;
		if(ly == LayoutHorizontal) new_ly = new QHBoxLayout();
		else new_ly = new QVBoxLayout();
		QLayout *old_ly = Super::layout();
		while(old_ly->count()) {
			QLayoutItem *lyt = old_ly->takeAt(0);
			new_ly->addItem(lyt);
		}
		//old_ly->setParent(nullptr);
		delete old_ly;
		setLayout(new_ly);
		emit layoutTypeChanged(ly);
	}
}

QQmlListProperty<QWidget> Frame::widgets()
{
	return QQmlListProperty<QWidget>(this,0,
                                    Frame::addWidgetFunction,
                                    Frame::countWidgetsFunction,
                                    Frame::widgetAtFunction,
                                    Frame::removeAllWidgetsFunction
                                    );
}

void Frame::addWidgetFunction(QQmlListProperty<QWidget> *list_property, QWidget *value)
{
    if (value)
    {
        Frame *that = static_cast<Frame*>(list_property->object);
        that->add(value);
    }    
}    

QWidget * Frame::widgetAtFunction(QQmlListProperty<QWidget> *list_property, int index)
{
    Frame *that = static_cast<Frame*>(list_property->object);
    return that->at(index);
}


void Frame::removeAllWidgetsFunction(QQmlListProperty<QWidget> *list_property)
{
    Frame *that = static_cast<Frame*>(list_property->object);
    that->removeAll();
}

int Frame::countWidgetsFunction(QQmlListProperty<QWidget> *list_property)
{
    Frame *that = static_cast<Frame*>(list_property->object);
    return that->count();
}

void Frame::add(QWidget *widget)
{
	if(widget) {
		qDebug() << "adding widget" << widget << widget->parent();
		widget->setParent(0);
		widget->setParent(this);
		Super::layout()->addWidget(widget);
		widget->show();
		//m_layout->addWidget(new QLabel("ahoj", this));
	}
}

QWidget *Frame::at(int index) const
{
	QList<QWidget*> lst = this->findChildren<QWidget*>();
	return lst.value(index);
}

void Frame::removeAll()
{
	QList<QWidget*> lst = this->findChildren<QWidget*>();
	qDeleteAll(lst);
}

int Frame::count() const
{
	QList<QWidget*> lst = this->findChildren<QWidget*>();
	return lst.count();
}
