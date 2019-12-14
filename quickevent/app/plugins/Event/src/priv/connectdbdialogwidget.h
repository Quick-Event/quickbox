#ifndef CONNECTDBDIALOGWIDGET_H
#define CONNECTDBDIALOGWIDGET_H

#include "../eventplugin.h"

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

	QString eventName();
	Event::EventPlugin::ConnectionType connectionType();
	QString serverHost();
	int serverPort();
	QString serverUser();
	QString serverPassword();

	void loadSettings();
	void saveSettings();
private:
	Q_SLOT void on_btSingleWorkingDirectory_clicked();
private:
	Ui::ConnectDbDialogWidget *ui;
};

#endif // CONNECTDBDIALOGWIDGET_H
