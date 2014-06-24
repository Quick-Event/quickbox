#include "frame.h"
#include "gridlayoutproperties.h"
#include "layoutpropertiesattached.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>

#include <QDebug>

using namespace qf::qmlwidgets;

static const int DefaultLayoutMargin = 1;

Frame::Frame(QWidget *parent) :
	Super(parent), m_layoutType(LayoutInvalid), m_gridLayoutProperties(nullptr)
{
}

Frame::LayoutType Frame::layoutType() const
{
	return m_layoutType;
}

void Frame::setLayoutType(Frame::LayoutType ly_type)
{
	qfLogFuncFrame() << ly_type;
	if(ly_type != layoutType()) {
		//QList<QLayoutItem*> layout_items;
		QLayout *old_ly = layout();
		if(old_ly) {
			delete old_ly;
		}
		m_layoutType = ly_type;
		for(auto w : m_childWidgets) {
			addToLayout(w);
		}
		qfDebug() << "new layout:" << layout();
		if(layout()) {
			// visualize changes in layouting
			layout()->activate();
		}
		emit layoutTypeChanged(ly_type);
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
    if (value) {
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
		m_childWidgets << widget;
		//Super::layout()->addWidget(widget);
		widget->show();
		//m_layout->addWidget(new QLabel("ahoj", this));
		addToLayout(widget);
	}
}

QWidget *Frame::at(int index) const
{
	return m_childWidgets.value(index);
}

void Frame::removeAll()
{
	qDeleteAll(m_childWidgets);
	m_childWidgets.clear();
}

int Frame::count() const
{
	return m_childWidgets.count();
}

void Frame::setGridLayoutProperties(GridLayoutProperties *props)
{
	m_gridLayoutProperties = props;
}

void Frame::addToLayout(QWidget *widget)
{
	qfLogFuncFrame();
	if(!layout()) {
		createLayout(layoutType());
		qfDebug() << "\tnew layout:" << layout() << this;
	}
	qfDebug() << "\tadding:" << widget << "to layout:" << layout() << this;
	{
		QGridLayout *ly = qobject_cast<QGridLayout*>(layout());
		if(ly) {
			GridLayoutProperties *props = gridLayoutProperties();
			GridLayoutProperties::Flow flow = GridLayoutProperties::LeftToRight;
			int cnt = -1;
			if(!props) {
				qfWarning() << this << "missing gridLayoutProperties property for GridLayout type";
			}
			else {
				flow = props->flow();
				cnt = (flow == GridLayoutProperties::LeftToRight)? props->columns(): props->rows();
			}
			if(cnt <= 0)
				cnt = 2;
			int row_span = 1, column_span = 1;
			LayoutPropertiesAttached *lpa = qobject_cast<LayoutPropertiesAttached*>(qmlAttachedPropertiesObject<LayoutProperties>(widget, false));
			if(lpa) {
				row_span = lpa->rowSpan();
				column_span = lpa->columnSpan();
			}
			if(flow == GridLayoutProperties::LeftToRight) {
				ly->addWidget(widget, m_currentLayoutRow, m_currentLayoutColumn, 1, column_span);
				m_currentLayoutColumn += column_span;
				if(m_currentLayoutColumn >= cnt) {
					m_currentLayoutColumn = 0;
					m_currentLayoutRow++;
				}
			}
			else {
				ly->addWidget(widget, m_currentLayoutRow, m_currentLayoutColumn, row_span, 1);
				m_currentLayoutRow += row_span;
				if(m_currentLayoutRow >= cnt) {
					m_currentLayoutRow = 0;
					m_currentLayoutColumn++;
				}
			}
			return;
		}
	}
	{
		QFormLayout *ly = qobject_cast<QFormLayout*>(layout());
		if(ly) {
			QString buddy_text;
			int column_span = 1;
			LayoutPropertiesAttached *lpa = qobject_cast<LayoutPropertiesAttached*>(qmlAttachedPropertiesObject<LayoutProperties>(widget, false));
			if(lpa) {
				column_span = lpa->columnSpan();
				buddy_text = lpa->buddyText();
			}
			if(column_span > 1) {
				ly->addRow(widget);
			}
			else if(!buddy_text.isEmpty()) {
				ly->addRow(buddy_text, widget);
			}
			else {
				ly->addWidget(widget);
			}
			return;
		}
	}
	{
		QBoxLayout *ly = qobject_cast<QBoxLayout*>(layout());
		if(ly) {
			ly->addWidget(widget);
			return;
		}
	}
}

void Frame::createLayout(LayoutType layout_type)
{
	QF_ASSERT(layout() == nullptr, "Form has layout already", return);
	m_currentLayoutColumn = m_currentLayoutRow = 0;
	QLayout *new_ly;
	switch(layout_type) {
	case LayoutGrid:
		new_ly = new QGridLayout();
		break;
	case LayoutForm:
		new_ly = new QFormLayout();
		break;
	case LayoutVertical:
		new_ly = new QVBoxLayout();
		break;
	default:
		new_ly = new QHBoxLayout();
		break;
	}
	new_ly->setMargin(DefaultLayoutMargin);
	setFrameShape(QFrame::Box);
	setLayout(new_ly);
}
