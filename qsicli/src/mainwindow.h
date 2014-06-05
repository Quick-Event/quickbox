#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui { class MainWindow; }
class SIMessageData;

class MainWindow : public QMainWindow
{
Q_OBJECT;
private:
	Ui::MainWindow *ui;
protected slots:
	void appendLog(int level, const QString &msg);
	void appendLogPre(int level, const QString &msg);
protected slots:
	void processDriverInfo(int level, const QString &msg);
	void processSIMessage(const SIMessageData &msg);
	void processDriverRawData(const QByteArray &data);
	void onCommOpen(bool checked);
	//void onSqlConnect(bool checked);
	void actConfigTriggered();
	void onHelpAbout();
	void onHelpAboutQt();
signals:
	void sendSICommand(int cmd, const QByteArray& data_params);
public:
	MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();
};

#endif // MAINWINDOW_H
