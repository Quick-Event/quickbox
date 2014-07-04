#include "partswitch.h"
#include "partwidget.h"
#include "stackedcentralwidget.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QIcon>

using namespace qf::qmlwidgets::framework;

PartSwitchToolButton::PartSwitchToolButton(QWidget *parent)
	: Super(parent)
{
	//setAutoFillBackground(false); /// musi bejt off
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	//setText(part->caption());
	//setIcon(part->icon());
	setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	setAutoRaise(true);
	setCheckable(true);
	setAutoExclusive(false);

	connect(this, &Super::clicked, [this]() {
		qfInfo() << "clicked" << this->m_partIndex;
		emit clicked(this->m_partIndex);
	});
}

PartSwitch::PartSwitch(StackedCentralWidget *central_widget, QWidget *parent) :
	Super(parent), m_centralWidget(central_widget), m_currentPartIndex(-1)
{
	setObjectName("partSwitch");
	//setStyleSheet("background-color: rgb(118, 118, 118);");
}


qf::qmlwidgets::framework::PartSwitch::~PartSwitch()
{
}

void PartSwitch::addPartWidget(PartWidget *widget)
{
	PartSwitchToolButton *bt = new PartSwitchToolButton();
	connect(bt, SIGNAL(clicked(int)), this, SLOT(setCurrentPartIndex(int)));
	bt->setText(widget->title());
	QIcon ico(":/qf/qmlwidgets/images/under-construction.png");
	bt->setIcon(ico);
	bt->setPartIndex(buttonCount());
	addWidget(bt);
}

void PartSwitch::setCurrentPartIndex(int ix)
{
	qfLogFuncFrame() << m_currentPartIndex << "->" << ix;
	if(m_currentPartIndex == ix)
		return;
	PartSwitchToolButton *bt1 = buttonAt(m_currentPartIndex);
	PartSwitchToolButton *bt2 = buttonAt(ix);
	bool ok1 = m_centralWidget->setPartActive(m_currentPartIndex, false);
	bool ok2 = false;
	if(ok1) {
		int old_ix = m_currentPartIndex;
		m_currentPartIndex = ix;
		ok2 = m_centralWidget->setPartActive(ix, true);
		if(!ok2) {
			m_currentPartIndex = old_ix;
		}
	}
	if(bt1 && ok2)
		bt1->setChecked(false);
	if(bt2)
		bt2->setChecked(ok2);
}

int PartSwitch::buttonCount()
{
	QList<PartSwitchToolButton*> lst = findChildren<PartSwitchToolButton*>(QString(), Qt::FindDirectChildrenOnly);
	return lst.count();
}

PartSwitchToolButton *PartSwitch::buttonAt(int part_index)
{
	PartSwitchToolButton *ret = nullptr;
	QList<PartSwitchToolButton*> lst = findChildren<PartSwitchToolButton*>(QString(), Qt::FindDirectChildrenOnly);
	for(auto bt : lst) {
		if(bt->partIndex() == part_index) {
			ret = bt;
			break;
		}
	}
	return ret;
}


