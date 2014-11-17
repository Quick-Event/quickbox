#include "frame.h"
//#include "LayoutTypeProperties.h"
#include "layouttypeproperties.h"
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
	Super(parent), m_layoutType(LayoutInvalid), m_layoutTypeProperties(nullptr)
{
}

Frame::~Frame()
{
	qDeleteAll(m_attachedObjects);
	m_attachedObjects.clear();
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

void Frame::setVisible(bool b)
{
	if(b != isVisible()) {
		Super::setVisible(b);
		emit visibleChanged(b);
	}
}

QQmlListProperty<QWidget> Frame::widgets()
{
	return QQmlListProperty<QWidget>(this, 0,
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
		/// widget cannot be simply reparented
		/// NULL parent should be set first
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

void Frame::setLayoutTypeProperties(LayoutTypeProperties *props)
{
	m_layoutTypeProperties = props;
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
			LayoutTypeProperties *props = layoutTypeProperties();
			LayoutTypeProperties::Flow flow = LayoutTypeProperties::LeftToRight;
			int cnt = -1;
			if(!props) {
				qfWarning() << this << "missing gridLayoutProperties property for GridLayout type";
			}
			else {
				flow = props->flow();
				cnt = (flow == LayoutTypeProperties::LeftToRight)? props->columns(): props->rows();
			}
			if(cnt <= 0)
				cnt = 2;
			int row_span = 1, column_span = 1;
			LayoutPropertiesAttached *lpa = qobject_cast<LayoutPropertiesAttached*>(qmlAttachedPropertiesObject<LayoutProperties>(widget, false));
			if(lpa) {
				row_span = lpa->rowSpan();
				column_span = lpa->columnSpan();
			}
			if(flow == LayoutTypeProperties::LeftToRight) {
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
			if(!buddy_text.isEmpty()) {
				qfDebug() << "\t add with buddy:" << buddy_text;
				ly->addRow(buddy_text, widget);
			}
			else if(column_span > 1) {
				qfDebug() << "\t add with col span:" << column_span;
				ly->addRow(widget);
			}
			else {
				qfDebug() << "\t add single widget";
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
	QBoxLayout *new_box_ly = nullptr;
	switch(layout_type) {
	case LayoutGrid:
		new_ly = new QGridLayout();
		break;
	case LayoutForm:
		new_ly = new QFormLayout();
		break;
	case LayoutHorizontal:
		new_ly = new_box_ly = new QHBoxLayout();
		break;
	default:
		new_ly = new_box_ly = new QVBoxLayout();
		break;
	}
	new_ly->setMargin(DefaultLayoutMargin);
	//setFrameShape(QFrame::Box);
	if(new_box_ly) {
		LayoutTypeProperties *props = layoutTypeProperties();
		if(props && props->spacing() >= 0)
			new_box_ly->setSpacing(props->spacing());
	}
	setLayout(new_ly);
}

QQmlListProperty<QObject> Frame::attachedObjects()
{
	return QQmlListProperty<QObject>(this, m_attachedObjects);
}
