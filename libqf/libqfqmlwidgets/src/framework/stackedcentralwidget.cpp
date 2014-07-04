#include "stackedcentralwidget.h"
#include "partwidget.h"
#include "mainwindow.h"
#include "partswitch.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QMetaMethod>

using namespace qf::qmlwidgets::framework;

StackedCentralWidget::StackedCentralWidget(MainWindow *parent) :
	Super(parent)
{
	qfLogFuncFrame();
	m_centralWidget = new QStackedWidget(this);
	QBoxLayout *ly = new QVBoxLayout(this);
	ly->setMargin(0);
	//qfDebug() << "\t adding:" << m_centralFrame << "to layout:" << ly;
	ly->addWidget(m_centralWidget);
	setLayout(ly);

	m_partSwitch = new PartSwitch(this);
	m_frameWork->addToolBar(Qt::LeftToolBarArea, m_partSwitch);
}

StackedCentralWidget::~StackedCentralWidget()
{
}

void StackedCentralWidget::addPartWidget(PartWidget *widget)
{
	widget->setParent(0);
	m_centralWidget->addWidget(widget);
	m_partSwitch->addPartWidget(widget);
}

bool StackedCentralWidget::setPartActive(int part_index, bool set_active)
{
	bool ret = true;
	PartWidget *pw = partWidget(part_index);
	if(pw) {
		QVariant ret_val(true);
		int ix = pw->metaObject()->indexOfMethod("canActivate(QVariant)");
		if(ix >= 0) {
			pw->metaObject()->method(ix).invoke(pw, Qt::DirectConnection,
												Q_RETURN_ARG(QVariant, ret_val),
											    Q_ARG(QVariant, set_active));
		}
		ret = ret_val.toBool();
		if(ret) {
			m_centralWidget->setCurrentIndex(part_index);
			emit partActivated(pw->featureId(), set_active);
		}
	}
	return ret;
}

PartWidget *StackedCentralWidget::partWidget(int part_index)
{
	PartWidget *pw = qobject_cast<PartWidget*>(m_centralWidget->widget(part_index));
	if(pw == nullptr) {
		//qfWarning() << "invalid part index:" << part_index << "of count:" << m_centralWidget->count();
	}
	return pw;
}
