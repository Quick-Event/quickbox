#include "partswitch.h"
#include "partwidget.h"
#include "stackedcentralwidget.h"
#include "mainwindow.h"
#include "plugin.h"
#include "pluginmanifest.h"
#include "pluginloader.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QIcon>

using namespace qf::qmlwidgets::framework;

PartSwitchToolButton::PartSwitchToolButton(QWidget *parent)
	: Super(parent)
{
	setAutoFillBackground(false); /// musi bejt off
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	setAutoRaise(true);
	setCheckable(true);
	setAutoExclusive(false);
	//setIconSize(QSize(64, 64));
	//setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	connect(this, &Super::clicked, [this]() {
		qfInfo() << "clicked" << this->m_partIndex;
		//this->setIconSize(this->size());
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
	bt->setPartIndex(buttonCount());
	addWidget(bt);
	QMetaObject::invokeMethod(this, "updateButtonIcon", Qt::QueuedConnection, Q_ARG(int, buttonCount() - 1));
}

void PartSwitch::updateButtonIcon(int part_index)
{
	PartSwitchToolButton *bt = buttonAt(part_index);
	PartWidget *pw = m_centralWidget->partWidget(part_index);
	if(bt && pw) {
		QIcon ico = pw->createIcon();
		//bt->setIconSize(bt->size());
		bt->setIcon(ico);
	}
}

void PartSwitch::setCurrentPartIndex(int ix, bool is_active)
{
	qfLogFuncFrame() << m_currentPartIndex << "->" << ix;
	if(!is_active)
		return;
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



