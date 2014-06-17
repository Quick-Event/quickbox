#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qf/qmlwidgets/framework/mainwindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public qf::qmlwidgets::framework::MainWindow
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::MainWindow Super;
public:
	explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
