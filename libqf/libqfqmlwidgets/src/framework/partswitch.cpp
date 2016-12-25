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
	: Super(parent), m_partIndex()
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
		//qfInfo() << "clicked" << this->m_partIndex;
		//this->setIconSize(this->size());
		emit clicked(this->m_partIndex);
	});
}

PartSwitch::PartSwitch(StackedCentralWidget *central_widget, QWidget *parent) :
	Super(parent), m_centralWidget(central_widget), m_currentPartIndex(-1)
{
	setObjectName("partSwitch");
	setWindowTitle(tr("Part switch"));
	//setStyleSheet("background-color: rgb(118, 118, 118);");
}


qf::qmlwidgets::framework::PartSwitch::~PartSwitch()
{
}

void PartSwitch::addPartWidget(PartWidget *widget)
{
	qfLogFuncFrame() << widget;
	auto alst = actions();
	QAction *first_bottom_part_action = nullptr;
	bool add_from_bottom = widget->isAddToPartSwitchFromBottom();
	if(!add_from_bottom) {
		for (int i = 0; i < alst.count(); ++i) {
			PartSwitchToolButton *bt = qobject_cast<PartSwitchToolButton*>(widgetForAction(alst.at(i)));
			if(bt) {
				PartWidget *pw = m_centralWidget->partWidget(bt->partIndex());
				if(pw->isAddToPartSwitchFromBottom()) {
					first_bottom_part_action = alst.at(i);
					break;
				}
			}
		}
	}
	PartSwitchToolButton *bt = new PartSwitchToolButton();
	bt->setCheckable(true);
	connect(bt, SIGNAL(clicked(int)), this, SLOT(setCurrentPartIndex(int)));
	bt->setText(widget->title());
	bt->setPartIndex(buttonCount());
	qfDebug() << "first_bottom_part_action:" << first_bottom_part_action << "add_from_bottom:" << add_from_bottom;
	insertWidget(first_bottom_part_action, bt);
	updateButtonIcon(bt);
	//QMetaObject::invokeMethod(this, "updateButtonIcon", Qt::QueuedConnection, Q_ARG(QObject*, bt));
}

void PartSwitch::updateButtonIcon(PartSwitchToolButton *bt)
{
	PartWidget *pw = m_centralWidget->partWidget(bt->partIndex());
	if(pw) {
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
	bool ok1 = m_centralWidget->setActivePart(m_currentPartIndex, false);
	bool ok2 = false;
	if(ok1) {
		int old_ix = m_currentPartIndex;
		m_currentPartIndex = ix;
		ok2 = m_centralWidget->setActivePart(ix, true);
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
	const QList<PartSwitchToolButton*> lst = findChildren<PartSwitchToolButton*>(QString(), Qt::FindDirectChildrenOnly);
	for(auto bt : lst) {
		if(bt->partIndex() == part_index) {
			ret = bt;
			break;
		}
	}
	return ret;
}



