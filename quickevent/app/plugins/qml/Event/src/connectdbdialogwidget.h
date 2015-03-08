#ifndef CONNECTDBDIALOGWIDGET_H
#define CONNECTDBDIALOGWIDGET_H

#include "eventplugin.h"

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class ConnectDbDialogWidget;
}

class ConnectDbDialogWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit ConnectDbDialogWidget(QWidget *parent = 0);
	~ConnectDbDialogWidget();

	EventPlugin::ConnectionType connectionType();
	QString serverHost();
	int serverPort();
	QString serverUser();
	QString serverPassword();

	void loadSettings();
	void saveSettings();
private:
	Ui::ConnectDbDialogWidget *ui;
};

#endif // CONNECTDBDIALOGWIDGET_H
