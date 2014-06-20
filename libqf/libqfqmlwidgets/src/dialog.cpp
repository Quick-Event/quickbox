#include "dialog.h"
#include "frame.h"

#include <QVBoxLayout>

using namespace qf::qmlwidgets;

Dialog::Dialog(QWidget *parent) :
	QDialog(parent)
{
	m_centralFrame = new Frame(this);
	m_centralFrame->setLayoutType(Frame::LayoutVertical);
	QBoxLayout *ly = new QVBoxLayout(this);
	ly->setMargin(1);
	ly->addWidget(m_centralFrame);
	setLayout(ly);
}

QQmlListProperty<QWidget> Dialog::widgets()
{
	return m_centralFrame->widgets();
}


