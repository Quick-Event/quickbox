#include "cardreadoutwidget.h"
#include "ui_cardreadoutwidget.h"

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <qf/core/log.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

CardReadoutWidget::CardReadoutWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CardReadoutWidget)
{
	ui->setupUi(this);

	createActions();

	qff::Application *app = qff::Application::instance();
	qff::MainWindow *fw = app->frameWork();

	qfw::Action *a = fw->menuBar()->actionForPath("view", false);
	if(!a) {
		qfError() << "View doesn't exist";
	}
	else {
		a = a->addMenuBefore("cards", "&Cards");
		a->addActionInto(m_actCommOpen);
		//a->addActionInto(m_actSqlConnect);
	}

	qfw::ToolBar *main_tb = fw->toolBar("main", true);
	main_tb->addAction(m_actCommOpen);
}

CardReadoutWidget::~CardReadoutWidget()
{
	delete ui;
}

void CardReadoutWidget::createActions()
{
	//QStyle *sty = style();
	{
		QIcon ico(":/quickevent/CardReadout/images/comm");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Open COM"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(commOpen(bool)));
		m_actCommOpen = a;
	}
	/*
	{
		QIcon ico(":/quickevent/CardReadout/images/sql");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Connect SQL"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(sqlConnect(bool)));
		m_actSqlConnect = a;
	}
	*/
}

