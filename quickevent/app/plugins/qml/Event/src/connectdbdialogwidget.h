#ifndef CONNECTDBDIALOGWIDGET_H
#define CONNECTDBDIALOGWIDGET_H

#include "eventplugin.h"

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class ConnectdbDialogWidget;
}

class ConnectdbDialogWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit ConnectdbDialogWidget(QWidget *parent = 0);
	~ConnectdbDialogWidget();

	EventPlugin::ConnectionType connectionType();
	QString serverHost();
	int serverPort();
	QString serverUser();
	QString serverPassword();

	void loadSettings();
	void saveSettings();
private:
	Ui::ConnectdbDialogWidget *ui;
};

#endif // CONNECTDBDIALOGWIDGET_H
