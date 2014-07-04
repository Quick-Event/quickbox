#include "partwidget.h"
#include "../frame.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QVBoxLayout>

using namespace qf::qmlwidgets::framework;

PartWidget::PartWidget(QWidget *parent) :
	Super(parent), IPersistentSettings(this)
{
	qfLogFuncFrame();
	m_centralFrame = new Frame(this);
	m_centralFrame->setLayoutType(Frame::LayoutVertical);
	QBoxLayout *ly = new QVBoxLayout(this);
	//ly->setMargin(1);
	qfDebug() << "\t adding:" << m_centralFrame << "to layout:" << ly;
	ly->addWidget(m_centralFrame);
	setLayout(ly);
}

PartWidget::~PartWidget()
{
}

void PartWidget::setTitle(QString arg)
{
	if (m_title != arg) {
		m_title = arg;
		emit titleChanged(arg);
	}
}

void PartWidget::setFeatureId(QString id)
{
	if (m_title != id) {
		m_title = id;
		emit featureIdChanged(id);
	}
}
/*
bool PartWidget::canActivate(bool active_on)
{
	qfLogFuncFrame() << this << active_on;
	Q_UNUSED(active_on);
	return true;
}
*/
QQmlListProperty<QWidget> PartWidget::widgets()
{
	return m_centralFrame->widgets();
}

void PartWidget::savePersistentSettings()
{
}

void PartWidget::classBegin()
{
}

void PartWidget::componentComplete()
{
}
