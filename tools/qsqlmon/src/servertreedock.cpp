#include "servertreedock.h"

ServerTreeDock::ServerTreeDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
	setObjectName("Connections tree");
	setWindowTitle(objectName());
	
	QWidget *w = new QWidget(this);
	ui.setupUi(w);
	setWidget(w);
}
